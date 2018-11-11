/**********************************************************************

Audacity: A Digital Audio Editor

WaveformPrefs.cpp

Paul Licameli

*******************************************************************//**

\class WaveformPrefs
\brief A PrefsPanel for spectrum settings.

*//*******************************************************************/

#include "../Audacity.h"
#include "WaveformPrefs.h"

#include "GUIPrefs.h"
#include "GUISettings.h"

#include <wx/checkbox.h>
#include <wx/choice.h>

#include "../Project.h"

#include "../TrackPanel.h"
#include "../ShuttleGui.h"
#include "../WaveTrack.h"

WaveformPrefs::WaveformPrefs(wxWindow * parent, wxWindowID winid, WaveTrack *wt)
/* i18n-hint: A waveform is a visual representation of vibration */
: PrefsPanel(parent, winid, _("Waveforms"))
, mWt(wt)
, mPopulating(false)
{
   if (mWt) {
      WaveformSettings &settings = wt->GetWaveformSettings();
      mDefaulted = (&WaveformSettings::defaults() == &settings);
      mTempSettings = settings;
   }
   else  {
      mTempSettings = WaveformSettings::defaults();
      mDefaulted = false;
   }

   mTempSettings.ConvertToEnumeratedDBRange();
   Populate();
}

WaveformPrefs::~WaveformPrefs()
{
}

enum {
   ID_DEFAULTS = 10001,

   ID_SCALE,
   ID_RANGE,
};

void WaveformPrefs::Populate()
{
   // Reuse the same choices and codes as for Interface prefs
   GUIPrefs::GetRangeChoices(&mRangeChoices, &mRangeCodes);

   //------------------------- Main section --------------------
   // Now construct the GUI itself.
   ShuttleGui S(this, eIsCreatingFromPrefs);
   PopulateOrExchange(S);
   // ----------------------- End of main section --------------
}

void WaveformPrefs::PopulateOrExchange(ShuttleGui & S)
{
   mPopulating = true;

   S.SetBorder(2);
   S.StartScroller();

   // S.StartStatic(_("Track Settings"));
   {
      mDefaultsCheckbox = 0;
      if (mWt) {
         /* i18n-hint: use is a verb */
         mDefaultsCheckbox = S.Id(ID_DEFAULTS).TieCheckBox(_("&Use Preferences"), mDefaulted);
      }

      S.StartStatic(_("Display"));
      {
         S.StartTwoColumn();
         {
            mScaleChoice =
               S.Id(ID_SCALE).TieChoice(_("S&cale") + wxString(wxT(":")),
                  mTempSettings.scaleType,
                  WaveformSettings::GetScaleNames());

            mRangeChoice =
               S.Id(ID_RANGE).TieChoice(_("Waveform dB &range") + wxString(wxT(":")),
               mTempSettings.dBRange,
               mRangeChoices);
         }
         S.EndTwoColumn();
      }
      S.EndStatic();
   }
   // S.EndStatic();

   /*
   S.StartStatic(_("Global settings"));
   {
   }
   S.EndStatic();
   */

   S.EndScroller();

   EnableDisableRange();

   mPopulating = false;
}

bool WaveformPrefs::Validate()
{
   // Do checking for whole numbers

   // ToDo: use wxIntegerValidator<unsigned> when available

   ShuttleGui S(this, eIsGettingFromDialog);
   PopulateOrExchange(S);

   // Delegate range checking to WaveformSettings class
   mTempSettings.ConvertToActualDBRange();
   const bool result = mTempSettings.Validate(false);
   mTempSettings.ConvertToEnumeratedDBRange();
   return result;
}

bool WaveformPrefs::Commit()
{
   const bool isOpenPage = this->IsShown();

   ShuttleGui S(this, eIsGettingFromDialog);
   PopulateOrExchange(S);

   mTempSettings.ConvertToActualDBRange();
   WaveformSettings::Globals::Get().SavePrefs();

   if (mWt) {
      for (auto channel : TrackList::Channels(mWt)) {
         if (mDefaulted)
            channel->SetWaveformSettings({});
         else {
            WaveformSettings &settings =
               channel->GetIndependentWaveformSettings();
            settings = mTempSettings;
         }
      }
   }

   if (!mWt || mDefaulted) {
      WaveformSettings *const pSettings =
         &WaveformSettings::defaults();
      *pSettings = mTempSettings;
      pSettings->SavePrefs();
   }

   mTempSettings.ConvertToEnumeratedDBRange();

   if (mWt && isOpenPage) {
      for (auto channel : TrackList::Channels(mWt))
         channel->SetDisplay(WaveTrack::Waveform);
   }

   if (isOpenPage) {
      TrackPanel *const tp = ::GetActiveProject()->GetTrackPanel();
      tp->UpdateVRulers();
      tp->Refresh(false);
   }

   return true;
}

bool WaveformPrefs::ShowsPreviewButton()
{
   return true;
}

void WaveformPrefs::OnControl(wxCommandEvent&)
{
   // Common routine for most controls
   // If any per-track setting is changed, break the association with defaults
   // Skip this, and View Settings... will be able to change defaults instead
   // when the checkbox is on, as in the original design.

   if (mDefaultsCheckbox && !mPopulating) {
      mDefaulted = false;
      mDefaultsCheckbox->SetValue(false);
   }
}

void WaveformPrefs::OnScale(wxCommandEvent &e)
{
   EnableDisableRange();

   // do the common part
   OnControl(e);
}

void WaveformPrefs::OnDefaults(wxCommandEvent &)
{
   if (mDefaultsCheckbox->IsChecked()) {
      mTempSettings = WaveformSettings::defaults();
      mTempSettings.ConvertToEnumeratedDBRange();
      mDefaulted = true;
      ShuttleGui S(this, eIsSettingToDialog);
      PopulateOrExchange(S);
   }
}

void WaveformPrefs::EnableDisableRange()
{
   mRangeChoice->Enable
      (mScaleChoice->GetSelection() == WaveformSettings::stLogarithmic);
}

BEGIN_EVENT_TABLE(WaveformPrefs, PrefsPanel)

EVT_CHOICE(ID_SCALE, WaveformPrefs::OnScale)
EVT_CHOICE(ID_RANGE, WaveformPrefs::OnControl)

EVT_CHECKBOX(ID_DEFAULTS, WaveformPrefs::OnDefaults)
END_EVENT_TABLE()

WaveformPrefsFactory::WaveformPrefsFactory(WaveTrack *wt)
: mWt(wt)
{
}

PrefsPanel *WaveformPrefsFactory::operator () (wxWindow *parent, wxWindowID winid)
{
   wxASSERT(parent); // to justify safenew
   return safenew WaveformPrefs(parent, winid, mWt);
}
