//
// Created by fatih on 11/26/17.
//

#include "texture.h"

namespace fst {
    Texture::Texture() {}
    Texture::Texture(int width,
                     int height,
                     unsigned char *image,
                     std::string imageName,
                     std::string interpolation,
                     std::string decalMode,
                     std::string appearance)
        : m_width(width),
          m_height(height),
          m_image(image),
          m_imageName(imageName),
          m_interpolation(interpolation),
          m_decalMode(decalMode),
          m_appearance(appearance)
    {
    }
}
