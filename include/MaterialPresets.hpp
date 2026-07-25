#pragma once

#include "Material.hpp"


inline const Material MIRROR{
    {0, 0, 0},
    {.04, .05, .04},
    {.1, .12, .1},
    {1, 1, 1},
    1.2f,
    0.f,
    .99f,
    600.f
};

inline const Material GLASS{
    {0, 0, 0},
    {.05, .05, .05},
    {.2, .2, .2},
    {.99, .99, .99},
    1.3f,
    .99f,
    .05f,
    500.f
};

inline const Material DIM_LIGHTBULB{
    {.35, .2, .06},
    {.6, .4, .15},
    {.2, .2, .15},
    {1, 1, 1},
    1.0f,
    0.f,
    .1f,
    80.f
};

inline const Material GREEN_GLOSSY{
    {0, 0, 0},
    {.05, .8, .1},
    {.75, .75, .75},
    {1, 1, 1},
    1.0f,
    0.f,
    .15f,
    300.f
};

inline const Material RED_GLOSSY{
    {0, 0, 0},
    {.8, .05, .05},
    {.75, .75, .75},
    {1, 1, 1},
    1.0f,
    0.f,
    .15f,
    300.f
};

inline const Material BLUE_GLOSSY{
    {0, 0, 0},
    {.05, .1, .8},
    {.75, .75, .75},
    {1, 1, 1},
    1.0f,
    0.f,
    .15f,
    300.f
};

inline const Material PURPLE_GLOSSY{
    {0, 0, 0},
    {.55, .08, .75},
    {.75, .75, .75},
    {1, 1, 1},
    1.0f,
    0.f,
    .15f,
    300.f
};

inline const Material CYAN_GLOSSY{
    {0, 0, 0},
    {.05, .75, .75},
    {.75, .75, .75},
    {1, 1, 1},
    1.0f,
    0.f,
    .15f,
    300.f
};

inline const Material YELLOW_GLOSSY{
    {0, 0, 0},
    {.8, .75, .05},
    {.75, .75, .75},
    {1, 1, 1},
    1.0f,
    0.f,
    .15f,
    300.f
};

inline const Material GREEN_MATTE{
    {0, 0, 0},
    {.05, .8, .1},
    {.03, .03, .03},
    {1, 1, 1},
    1.0f,
    0.f,
    0.f,
    20.f
};

inline const Material RED_MATTE{
    {0, 0, 0},
    {.8, .05, .05},
    {.03, .03, .03},
    {1, 1, 1},
    1.0f,
    0.f,
    0.f,
    20.f
};

inline const Material BLUE_MATTE{
    {0, 0, 0},
    {.05, .1, .8},
    {.03, .03, .03},
    {1, 1, 1},
    1.0f,
    0.f,
    0.f,
    20.f
};

inline const Material WHITE_MATTE{
    {0, 0, 0},
    {.9, .9, .9},
    {.03, .03, .03},
    {1, 1, 1},
    1.0f,
    0.f,
    0.f,
    20.f
};

inline const Material PURPLE_MATTE{
    {0, 0, 0},
    {.55, .08, .75},
    {.03, .03, .03},
    {1, 1, 1},
    1.0f,
    0.f,
    0.f,
    20.f
};

inline const Material CYAN_MATTE{
    {0, 0, 0},
    {.05, .75, .75},
    {.03, .03, .03},
    {1, 1, 1},
    1.0f,
    0.f,
    0.f,
    20.f
};

inline const Material YELLOW_MATTE{
    {0, 0, 0},
    {.8, .75, .05},
    {.03, .03, .03},
    {1, 1, 1},
    1.0f,
    0.f,
    0.f,
    20.f
};

inline const Material DEFAULT = MIRROR;
