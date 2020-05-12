#pragma once

namespace Library
{

    class  Layer
    {
        public:
            Layer();
            ~Layer();

            virtual void onAttach() = 0;
            virtual void onDetach() = 0;

            virtual void Update(float frameTime) = 0;
            virtual void Render() = 0;
        private:
    };

}
