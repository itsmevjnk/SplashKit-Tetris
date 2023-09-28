#ifndef COLOR_HSB_H
#define COLOR_HSB_H

#include "splashkit.h"

/**
 * @brief HSB colour object.
 * 
 */
class color_hsb {
    public:
        /**
         * @brief The colour's hue.
         * 
         */
        double hue;
        
        /**
         * @brief The colour's saturation.
         * 
         */
        double saturation;
        
        /**
         * @brief The colour's brightness.
         * 
         */
        double brightness;

        /**
         * @brief The colour's alpha (0-255).
         * 
         */
        int alpha;

        /**
         * @brief Construct a new HSB color object given the individual hue, saturation, brightness and alpha values.
         * 
         * @param h The colour's hue.
         * @param s The colour's saturation.
         * @param b The colour's brightness.
         * @param a The colour's alpha.
         */
        color_hsb(double h = 0, double s = 0, double b = 0, int a = 255);

        /**
         * @brief Construct a new HSB color object from a SplashKit color struct.
         * 
         * @param c The SplashKit color structure.
         */
        color_hsb(const color c);

        /**
         * @brief Overload function for typecasting the HSB color object to SplashKit color struct.
         * 
         * @return color The SplashKit color object.
         */
        operator color() const;

        /**
         * @brief Overload function for adding two HSB colours together (not colour mixing).
         * 
         * @param c2 The colour to be added with.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator+(const color_hsb &c2);

        /**
         * @brief Alias for operator+.
         * 
         * @param c2 The colour to be added with.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator+=(const color_hsb &c2);

        /**
         * @brief Overload function for subtracting HSB colours (not colour mixing).
         * 
         * @param c2 The colour to be subtracted from.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator-(const color_hsb &c2);

        /**
         * @brief Alias for operator-.
         * 
         * @param c2 The colour to be subtracted from.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator-=(const color_hsb &c2);

        /**
         * @brief Overload function for multiplying HSB colours.
         * 
         * @param c2 The colour to be multiplied with.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator*(const color_hsb &c2);

        /**
         * @brief Alias for operator*(color_hsb).
         * 
         * @param c2 The colour to be multiplied with.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator*=(const color_hsb &c2);

        /**
         * @brief Overload function for dividing HSB colours.
         * 
         * @param c2 The colour to be divided by.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator/(const color_hsb &c2);

        /**
         * @brief Alias for operator/(color_hsb).
         * 
         * @param c2 The colour to be divided by.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator/=(const color_hsb &c2);

        /**
         * @brief Overload function for changing brightness by multiplying with a numeric value.
         * 
         * @param k The colour brightness multiplier.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator*(const double &k);

        /**
         * @brief Alias for operator*(double).
         * 
         * @param k The colour brightness multiplier.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator*=(const double &k);

        /**
         * @brief Overload function for changing brightness by dividing by a numeric value.
         * 
         * @param k The colour brightness divider.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator/(const double &k);

        /**
         * @brief Alias for operator/(double).
         * 
         * @param k The colour brightness divider.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& operator/=(const double &k);

        /**
         * @brief Normalise the HSB color object's values.
         * 
         */
        void normalise();

        /**
         * @brief Multiply the colour's values by a multiplier.
         * 
         * @param k The multiplier.
         * @return color_hsb& The resulting HSB colour.
         */
        color_hsb& multiply_values(double k);
    
    private:
        /**
         * @brief Internal function for operator+.
         * 
         * @param c2 The color to be added.
         * @return color_hsb& The resulting color.
         */
        color_hsb& do_add(const color_hsb &c2);

        /**
         * @brief Internal function for operator-.
         * 
         * @param c2 The color to be subtracted from.
         * @return color_hsb& The resulting color.
         */
        color_hsb& do_sub(const color_hsb &c2);

        /**
         * @brief Internal function for operator*(color_hsb).
         * 
         * @param c2 The color to be multiplied with.
         * @return color_hsb& The resulting color.
         */
        color_hsb& do_mul_color(const color_hsb &c2);

        /**
         * @brief Internal function for operator/(color_hsb).
         * 
         * @param c2 The color to be divided by.
         * @return color_hsb& The resulting color.
         */
        color_hsb& do_div_color(const color_hsb &c2);

        /**
         * @brief Internal function for operator*(double).
         * 
         * @param k The color multiplier.
         * @return color_hsb& The resulting color.
         */
        color_hsb& do_mul_double(const double &k);

        /**
         * @brief Internal function for operator/(double).
         * 
         * @param k The color divider.
         * @return color_hsb& The resulting color.
         */
        color_hsb& do_div_double(const double &k);
};

#endif