#include <substrate.h>

#include <UIKit/UIKit.h>
#include <QuartzCore/CALayer.h>

#include <MapKit/MKMapView.h>
#include <MapKit/MKMapView-MKHeadingAdditions.h>
#include <MapKit/MKMapViewInternal.h>
#include <MapKit/MKMapViewHeadingChange.h>
#include <MapKit/MKMapView-UserPositioningInternal.h>
#include <MapKit/MKUserLocationLayer.h>
#include <MapKit/MKUserLocationView.h>

MSClassHook(UIView)
MSClassHook(MKMapViewHeadingChange)
MSClassHook(MKUserLocationLayer)
MSClassHook(MKMapView)
MSMetaClassHook(MKMapViewHeadingChange)

MSClassMessageHook0(id, MKMapViewHeadingChange, rotationAnimation) {
    return nil;
}

volatile bool state_;

MSInstanceMessageHook1(void, MKUserLocationLayer, setAffineTransform, CGAffineTransform, transform) {
    if (state_)
        MSOldCall(transform);
}

MSInstanceMessageHook0(BOOL, MKMapView, shouldRotateForHeading) {
    MSIvarHook(MKMapViewInternal *, _internal);
    double &heading(MSHookIvar<double>(_internal, "heading"));
    MKUserLocationView *location([self userLocationView]);
    state_ = true;
    [location rotateToDegrees:heading];
    state_ = false;
    return NO;
}
