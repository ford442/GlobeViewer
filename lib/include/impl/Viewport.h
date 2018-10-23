#pragma once

#include <map>
#include <tuple>

#include <boost/signals2.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "LoadGL.h"
#include "type/ViewData.h"


namespace gv {


class Viewport
{
public:
    Viewport();
    ~Viewport();

    void resize( int w, int h );
    void move( int x, int y );
    void zoom( int steps );
    void center();

    ViewData viewData() const;
    float unitInMeter() const;
    float meterInPixel() const;

    glm::mat4 projection() const;
    std::tuple<double, double> metersAtPixel( int x, int y ) const;

    boost::signals2::signal<void( ViewData )> viewUpdated;

private:
    void setProjection();
    int mapZoomLevel( int tileWidth ) const;

    int pixelW_;    //!< Width of viewport (in pixels)
    int pixelH_;    //!< Height of viewport (in pixels)
    float unitX_;   //!< Coordinate x of base point of viewport (in gl units)
    float unitY_;   //!< Coordinate y of base point of viewport (in gl units)
    float unitW_;   //!< Width of viewport (in gl units)
    float unitH_;   //!< Height of viewport (in gl units)
    float panX_;    //!< Pannig by coordinate x (in gl units)
    float panY_;    //!< Pannig by coordinate y (in gl units)

    static const float unitInMeter_;    //!< gl units in one meter
    float meterInPixel_;                //!< meters in one pixel
    float unitInPixel_;                 //!< gl units in one pixel

    static const float minLen_;                     //!< minimum length to display in 1920 pixels
    static const float maxLen_;                     //!< maximum Globe diameter plus 20% of it used as free space
    static const float minUnitInPixel_;             //!< minimum value of unitInPixel_
    static const float maxUnitInPixel_;             //!< maximum value of unitInPixel_
    static const std::map<float, float> zoomMap_;   //!< change of unitInPixel_ in one step depending on current zoom

    const GLfloat zNear_;
    const GLfloat zFar_;
    const GLfloat zCamera_;
    glm::mat4 proj_;
};


}
