#pragma once

#include "Material.hpp"


inline const Material MIRROR{
    {0, 0, 0},
    {.8, .9, .8},
    {.1, .12, .1},
    {.0, .0, .0},
    1.2f,
    {.9, .9, .9},
    600
};

inline const Material DEFAULT = MIRROR;

inline const Material GLASS{
    {0, 0, 0},
    {.05, .05, .05},
    {.2, .2, .2},
    {.99, .99, .99},
    1.3f,
    {.05, .05, .05},
    500
};

inline const Material DIM_LIGHTBULB{
    {.35, .2, .06},
    {.6, .4, .15},
    {.2, .2, .15},
    {0, 0, 0},
    1.0f,
    {.05, .05, .05},
    80
};

inline const Material GREEN_GLOSSY{
    {0, 0, 0},
    {.05, .8, .1},
    {.75, .75, .75},
    {0, 0, 0},
    1.0f,
    {.15, .15, .15},
    300
};

inline const Material RED_GLOSSY{
    {0, 0, 0},
    {.8, .05, .05},
    {.75, .75, .75},
    {0, 0, 0},
    1.0f,
    {.15, .15, .15},
    300
};

inline const Material BLUE_GLOSSY{
    {0, 0, 0},
    {.05, .1, .8},
    {.75, .75, .75},
    {0, 0, 0},
    1.0f,
    {.15, .15, .15},
    300
};

inline const Material PURPLE_GLOSSY{
    {0, 0, 0},
    {.55, .08, .75},
    {.75, .75, .75},
    {0, 0, 0},
    1.0f,
    {.15, .15, .15},
    300
};

inline const Material CYAN_GLOSSY{
    {0, 0, 0},
    {.05, .75, .75},
    {.75, .75, .75},
    {0, 0, 0},
    1.0f,
    {.15, .15, .15},
    300
};

inline const Material YELLOW_GLOSSY{
    {0, 0, 0},
    {.8, .75, .05},
    {.75, .75, .75},
    {0, 0, 0},
    1.0f,
    {.15, .15, .15},
    300
};

inline const Material GREEN_MATTE{
    {0, 0, 0},
    {.05, .8, .1},
    {.03, .03, .03},
    {0, 0, 0},
    1.0f,
    {0, 0, 0},
    20
};

inline const Material RED_MATTE{
    {0, 0, 0},
    {.8, .05, .05},
    {.03, .03, .03},
    {0, 0, 0},
    1.0f,
    {0, 0, 0},
    20
};

inline const Material BLUE_MATTE{
    {0, 0, 0},
    {.05, .1, .8},
    {.03, .03, .03},
    {0, 0, 0},
    1.0f,
    {0, 0, 0},
    20
};

inline const Material PURPLE_MATTE{
    {0, 0, 0},
    {.55, .08, .75},
    {.03, .03, .03},
    {0, 0, 0},
    1.0f,
    {0, 0, 0},
    20
};

inline const Material CYAN_MATTE{
    {0, 0, 0},
    {.05, .75, .75},
    {.03, .03, .03},
    {0, 0, 0},
    1.0f,
    {0, 0, 0},
    20
};

inline const Material YELLOW_MATTE{
    {0, 0, 0},
    {.8, .75, .05},
    {.03, .03, .03},
    {0, 0, 0},
    1.0f,
    {0, 0, 0},
    20
};
