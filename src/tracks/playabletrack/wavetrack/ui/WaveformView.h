/**********************************************************************

Audacity: A Digital Audio Editor

WaveformView.h

Paul Licameli split from WaveTrackView.h

**********************************************************************/

#ifndef __AUDACITY_WAVEFORM_VIEW__
#define __AUDACITY_WAVEFORM_VIEW__

#include "../../../ui/CommonTrackView.h" // to inherit

class WaveTrack;

class WaveformView final : public CommonTrackView
{
   WaveformView( const WaveformView& ) = delete;
   WaveformView &operator=( const WaveformView& ) = delete;

public:
   using CommonTrackView::CommonTrackView;
   ~WaveformView() override;

   std::shared_ptr<TrackVRulerControls> DoGetVRulerControls() override;


private:
   // TrackPanelDrawable implementation
   void Draw(
      TrackPanelDrawingContext &context,
      const wxRect &rect, unsigned iPass ) override;

   std::vector<UIHandlePtr> DetailedHitTest(
      const TrackPanelMouseState &state,
      const AudacityProject *pProject, int currentTool, bool bMultiTool )
      override;
   static std::vector<UIHandlePtr> DoDetailedHitTest(
      const TrackPanelMouseState &state,
      const AudacityProject *pProject, int currentTool, bool bMultiTool,
      const std::shared_ptr<WaveTrack> &wt,
      CommonTrackView &view);

protected:
   void DoSetMinimized( bool minimized ) override;

   friend class WaveTrackView;
};

#endif
