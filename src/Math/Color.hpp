#pragma once

struct Color
{
  union
  {
    struct
    {
      uint8 a, b, g, r;
    };
    uint32 data = 0;
  };

  Color() = default;

  Color(uint32 color)
    : data(color)
  {
  }

  Color(uint8 r, uint8 g, uint8 b, uint8 a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
  {
  }
};
