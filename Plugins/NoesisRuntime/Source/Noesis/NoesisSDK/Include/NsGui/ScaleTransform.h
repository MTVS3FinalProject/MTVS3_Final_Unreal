////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __GUI_SCALETRANSFORM_H__
#define __GUI_SCALETRANSFORM_H__


#include <NsCore/Noesis.h>
#include <NsGui/Transform.h>


namespace Noesis
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Scales an object in the 2D coordinate system.
///
/// http://msdn.microsoft.com/en-us/library/system.windows.media.scaletransform.aspx
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_GUI_CORE_API ScaleTransform final: public Transform
{
public:
    ScaleTransform();
    ScaleTransform(float scaleX, float scaleY);
    ~ScaleTransform();

    NS_DECLARE_POOL_ALLOCATOR

    /// Gets or sets the x-axis scale factor
    //@{
    float GetScaleX() const;
    void SetScaleX(float scaleX);
    //@}
    
    /// Gets or sets the y-axis scale factor
    //@{
    float GetScaleY() const;
    void SetScaleY(float scaleY);
    //@}
    
    /// Gets or sets the x-coordinate of the scale center point
    //@{
    float GetCenterX() const;
    void SetCenterX(float centerX);
    //@}
    
    /// Gets or sets the y-coordinate of the scale center point
    //@{
    float GetCenterY() const;
    void SetCenterY(float centerY);
    //@}

    // Hides Freezable methods for convenience
    //@{
    Ptr<ScaleTransform> Clone() const;
    Ptr<ScaleTransform> CloneCurrentValue() const;
    //@}

public:
    /// Dependency properties
    //@{
    static const DependencyProperty* CenterXProperty;
    static const DependencyProperty* CenterYProperty;
    static const DependencyProperty* ScaleXProperty;
    static const DependencyProperty* ScaleYProperty;
    //@}

protected:
    /// From Freezable
    //@{
    Ptr<Freezable> CreateInstanceCore() const override;
    //@}

    NS_DECLARE_REFLECTION(ScaleTransform, Transform)
};

}

#endif
