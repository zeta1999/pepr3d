#pragma once

#include "cinder/Utilities.h"
#include "cinder/gl/gl.h"
#include "glm/glm.hpp"

#include "peprimgui.h"

#include "ui/CameraUi.h"

#include <chrono>
#include "geometry/TrianglePrimitive.h"

namespace pepr3d {

class MainApplication;

/// The main part of the user interface, shows the geometry to the user
class ModelView {
   public:
    explicit ModelView(MainApplication& app) : mApplication(app) {}

    // TODO: Hide this behind some api
    std::vector<glm::vec3> previewTriangles;
    std::vector<uint32_t> previewIndices;
    std::vector<glm::vec4> previewColors;
    std::vector<glm::vec3> previewNormals;

    void resetPreview() {
        previewTriangles.clear();
        previewIndices.clear();
        previewColors.clear();
        previewNormals.clear();
    }

    /// Setups the camera and shaders. Call only once!
    void setup();

    /// Called when the ModelView should recalculate its size.
    void resize();

    /// Draws the ModelView, both ImGui and the Geometry.
    void draw();

    /// Draws the Geometry.
    void drawGeometry();

    /// On mouse-down event over the ModelView area.
    void onMouseDown(ci::app::MouseEvent event);

    /// On mouse-drag event over the ModelView area.
    void onMouseDrag(ci::app::MouseEvent event);

    /// On mouse-up event over the ModelView area.
    void onMouseUp(ci::app::MouseEvent event);

    /// On mouse-wheel event over the ModelView area.
    void onMouseWheel(ci::app::MouseEvent event);

    /// On mouse-move event over the ModelView area.
    void onMouseMove(ci::app::MouseEvent event);

    /// Returns a 3D ray in the Geometry scene computed from window coordinates.
    ci::Ray getRayFromWindowCoordinates(glm::ivec2 windowCoords) const;

    /// Draws a highlight (3 lines with a contrasting color) of a specified DetailedTriangleId.
    void drawTriangleHighlight(const DetailedTriangleId triangleId);

    /// Draws a highlight (3 lines with a contrasting color) of a specified triangle ID.
    void drawTriangleHighlight(const size_t triangleIdx) {
        drawTriangleHighlight(DetailedTriangleId(triangleIdx));
    }

    /// Draws a 3D line.
    void drawLine(const glm::vec3& from, const glm::vec3& to, const ci::Color& color = ci::Color::white(),
                  float width = 1.0f, bool depthTest = false);

    /// Draws a text label (caption) to the top left corner.
    /// `caption` is black, `errorCaption` is red and below `caption`
    void drawCaption(const std::string& caption, const std::string& errorCaption);

    /// Returns true if wireframe of Geometry is rendered.
    bool isWireframeEnabled() const {
        return mIsWireframeEnabled;
    }

    /// Sets whether the wireframe of Geometry should be rendered.
    void enableWireframe(bool enable = true) {
        mIsWireframeEnabled = enable;
    }

    /// Returns true if the grid below the Geometry is rendered.
    bool isGridEnabled() const {
        return mIsGridEnabled;
    }

    /// Sets whether the grid below the Geometry should be rendered.
    void enableGrid(bool enable = true) {
        mIsGridEnabled = enable;
    }

    /// Returns the current roll (rotation in the axis not controlled by CameraUi) of the Geometry object.
    float getModelRoll() const {
        return mModelRoll;
    }

    /// Sets the current roll (rotation in the axis not controlled by CameraUi) of the Geometry object.
    void setModelRoll(float roll) {
        mModelRoll = roll;
    }

    /// Gets the 3D translation of the Geometry object.
    glm::vec3 getModelTranslate() const {
        return mModelTranslate;
    }

    /// Sets the 3D translation of the Geometry object.
    void setModelTranslate(glm::vec3 translate) {
        mModelTranslate = translate;
    }

    /// Resets the camera to the default position.
    void resetCamera();

    /// Resets the camera and sets zoom to be field-of-view based (so on zoom, the camera position does not change, only
    /// FOV).
    void setFovZoomEnabled(bool enabled) {
        resetCamera();
        mCameraUi.setFovZoomEnabled(enabled);
    }

    /// Returns true if the camera zoom is field-of-view based (so on zoom, the camera position does not change, only
    /// FOV).
    bool isFovZoomEnabled() const {
        return mCameraUi.isFovZoomEnabled();
    }

    /// Enables or disables vertex, normal, and index buffer override (all at once)
    void toggleMeshOverride(bool newState) {
        mMeshOverride.isOverriden = newState;
        forceBatchRefresh();
    }

    /// Batch data is going to be created again before rendering new frame. This will apply the overriden mesh.
    void forceBatchRefresh() {
        mBatch = nullptr;  // reset batch to force update
    }

    /// Returns a reference to the override color buffer, so you can read it or write to it.
    std::vector<glm::vec4>& getOverrideColorBuffer() {
        return mMeshOverride.overrideColorBuffer;
    }

    /// Returns true if the mesh data is overriden. (A mesh different from the geometry is being displayed)
    bool isMeshOverriden() const {
        return mMeshOverride.isOverriden;
    }

    /// Returns a reference to the override vertex buffer, so you can read it or write to it.
    std::vector<glm::vec3>& getOverrideVertexBuffer() {
        return mMeshOverride.overrideVertexBuffer;
    }

    /// Returns a reference to the override normal buffer, so you can read it or write to it.
    std::vector<glm::vec3>& getOverrideNormalBuffer() {
        return mMeshOverride.overrideNormalBuffer;
    }

    /// Returns a reference to the override index buffer, so you can read it or write to it.
    std::vector<uint32_t>& getOverrideIndexBuffer() {
        return mMeshOverride.overrideIndexBuffer;
    }

    /// Initialize override buffer to data from non-detailed geometry.
    void initOverrideFromBasicGeoemtry();

    /// Returns the minimum (first) and maximum (second) height that is rendered in the ModelView.
    glm::vec2 getPreviewMinMaxHeight() const {
        return mPreviewMinMaxHeight;
    }

    /// Sets the minimum (first) and maximum (second) height that is rendered in the ModelView.
    void setPreviewMinMaxHeight(glm::vec2 minMax) {
        mPreviewMinMaxHeight = minMax;
        mModelShader->uniform("uPreviewMinMaxHeight", mPreviewMinMaxHeight);
    }

    /// Returns the maximum size (in the X, Y, Z axes) of the current Geometry object.
    float getMaxSize() const {
        return mMaxSize;
    }

    /// Called by MainAplication when new geometry is loaded
    void onNewGeometryLoaded();

    glm::mat4 getModelMatrix() const {
        return mModelMatrix;
    }

    const ci::CameraPersp& getCamera() const {
        return mCamera;
    }

   private:
    MainApplication& mApplication;
    ci::gl::VboMeshRef mVboMesh;
    ci::gl::BatchRef mBatch;

    std::pair<glm::ivec2, glm::ivec2> mViewport;
    ci::CameraPersp mCamera;
    pepr3d::CameraUi mCameraUi;
    ci::gl::GlslProgRef mModelShader;
    bool mIsWireframeEnabled = false;
    bool mIsGridEnabled = true;
    float mGridOffset = 0.0f;
    glm::mat4 mModelMatrix;
    float mModelRoll = 0.0f;
    glm::vec3 mModelTranslate = glm::vec3(0);
    float mMaxSize = 1.f;
    glm::vec2 mPreviewMinMaxHeight = glm::vec2(0.0f, 1.0f);

    /// Override currently displayed mesh data, making it possibly to change displayed mesh
    /// without chaning the geometry itself
    struct MeshDataOverride {
        bool isOverriden = false;
        std::vector<glm::vec3> overrideVertexBuffer;
        std::vector<glm::vec3> overrideNormalBuffer;
        std::vector<uint32_t> overrideIndexBuffer;
        std::vector<glm::vec4> overrideColorBuffer;
    } mMeshOverride;

    /// Recalculates the model matrix of the current Geometry object.
    /// The model matrix ensures that the object's maximum displayed size is 1.0 and it is centered above the grid,
    /// touching it on the bottom.
    void updateModelMatrix();

    /// Recalculates the OpenGL vertex buffer object and the Cinder batch.
    void updateVboAndBatch();

    /// Custom OpenGL attributes
    struct Attributes {
        static const cinder::geom::Attrib COLOR_IDX = cinder::geom::Attrib::CUSTOM_0;
        static const cinder::geom::Attrib HIGHLIGHT_MASK = cinder::geom::Attrib::CUSTOM_1;
    };
};

}  // namespace pepr3d