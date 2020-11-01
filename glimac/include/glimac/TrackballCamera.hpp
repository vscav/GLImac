class TrackballCamera {
    private :
    
        float m_fDistance; // Distance from the center of the scene
        float m_fAngleX; // Angle made by the camera around the x axis of the scene
        float m_fAngleY; // angle made by the camera around the y axis of the scene

    public :

        TrackballCamera();
        ~TrackballCamera();

        void moveFront(float delta);
        void rotateLeft(float degrees);
        void rotateUp(float degrees);

        glm::mat4 getViewMatrix() const
};