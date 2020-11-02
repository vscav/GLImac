class FreeflyCamera {
    private :
    
        glm::vec3 m_Position; // Camera position

        // Front vector spherical coordinates
        float m_fPhi;
        float m_fTheta;

        glm::vec3 m_FrontVector; // Front vector
        glm::vec3 m_LeftVector; // Left vector
        glm::vec3 m_UpVector; // Up vector

        // Calcul of Front, Left and Up vectors
        void computeDirectionVectors();

    public :

        FreeflyCamera();
        ~FreeflyCamera();

        void moveLeft(float t);
        void moveFront(float t);

        void rotateLeft(float degrees);
        void rotateUp(float degrees);

        glm::mat4 getViewMatrix() const;
};