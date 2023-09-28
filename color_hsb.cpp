#include "color_hsb.h"

/* manual HSB + alpha constructor */
color_hsb::color_hsb(double h, double s, double b, int a) {
    hue = h;
    saturation = s;
    brightness = b;
    alpha = a;
}

/* SplashKit color constructor */
color_hsb::color_hsb(const color c) {
    hue = hue_of(c);
    saturation = saturation_of(c);
    brightness = brightness_of(c);
    alpha = alpha_of(c);
}

/* convert HSB color to SplashKit color */
color_hsb::operator color() const {
    color result = hsb_color(hue, saturation, brightness);
    result.a = alpha / 255.0;
    return result;
}

/* normalise color values */
void color_hsb::normalise() {
    /* apply hue normalisation */
    while(hue < 0.0) hue += 1.0;
    while(hue > 1.0) hue -= 1.0;

    /* check bounds of the remaining values */
    if(saturation < 0) saturation = 0;
    else if(saturation > 1) saturation = 1;
    if(brightness < 0) brightness = 0;
    else if(brightness > 1) brightness = 1;
    if(alpha < 0) alpha = 0;
    else if(alpha > 1) alpha = 1;
}

color_hsb& color_hsb::operator+(const color_hsb &c2) {
    hue += c2.hue;
    saturation += c2.saturation;
    brightness += c2.brightness;
    alpha += c2.alpha;
    normalise();
    return *this;
}

color_hsb& color_hsb::operator-(const color_hsb &c2) {
    hue -= c2.hue;
    saturation -= c2.saturation;
    brightness -= c2.brightness;
    alpha -= c2.alpha;
    normalise();
    return *this;
}

color_hsb& color_hsb::operator*(const color_hsb &c2) {
    hue *= c2.hue;
    saturation *= c2.saturation;
    brightness *= c2.brightness;
    alpha *= c2.alpha;
    normalise();
    return *this;
}

color_hsb& color_hsb::operator/(const color_hsb &c2) {
    hue /= c2.hue;
    saturation /= c2.saturation;
    brightness /= c2.brightness;
    alpha /= c2.alpha;
    normalise();
    return *this;
}

color_hsb& color_hsb::operator*(const double &k) {
    alpha *= k;
    if(alpha < 0) alpha = 0;
    else if(alpha > 1) alpha = 1;
    return *this;
}

color_hsb& color_hsb::operator/(const double &k) {
    alpha /= k;
    if(alpha < 0) alpha = 0;
    else if(alpha > 1) alpha = 1;
    return *this;
}