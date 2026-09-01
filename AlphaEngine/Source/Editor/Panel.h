#pragma once

namespace Alpha::Editor
{

    class Panel
    {
    public:
        virtual ~Panel() = default;

        virtual void Draw() = 0;
    };

}