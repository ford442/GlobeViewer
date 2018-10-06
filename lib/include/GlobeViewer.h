#pragma once

#include <memory>


namespace gv {


class GlobeViewer
{
public:
    GlobeViewer();
    ~GlobeViewer();

    bool validSetup() const;
    void render();

    void resize( int w, int h );
    void move( int x, int y );
    void zoom( int steps );
    void centerView();
    void rotate( int x, int y );

    /*! \brief  Call this at the end of the main function
     *          if an instance of GlobeViewer is a global variable.
     *          Otherwise it will conflict during destruction of
     *          static variables used in GlobeViewer and may crash!
     */
    void cleanup();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};


}
