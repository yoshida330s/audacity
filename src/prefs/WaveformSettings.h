/**********************************************************************

Audacity: A Digital Audio Editor

WaveformSettings.h

Paul Licameli

**********************************************************************/

#ifndef __AUDACITY_WAVEFORM_SETTINGS__
#define __AUDACITY_WAVEFORM_SETTINGS__

class WaveformSettings
{
public:

   // Singleton for settings that are not per-track
   class Globals
   {
   public:
      static Globals &Get();
      void SavePrefs();

   private:
      Globals();
      void LoadPrefs();
   };

   static WaveformSettings &defaults();
   WaveformSettings();
   WaveformSettings(const WaveformSettings &other);
   WaveformSettings& operator= (const WaveformSettings &other);
   ~WaveformSettings();

   bool IsDefault() const
   {
      return this == &defaults();
   }

   bool Validate(bool quiet);
   void LoadPrefs();
   void SavePrefs();
   void Update();
};
#endif
