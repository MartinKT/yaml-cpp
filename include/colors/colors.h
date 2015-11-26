#pragma once 

#include <stdint.h>
#ifdef _WIN32
#   include <io.h>
#else
#   include <unistd.h>
#   define _isatty isatty
#   define _fileno fileno
#endif

//// C++ standard string
#include <string>

/**
@file colors.h


*/

namespace colors {
    /**
        Supported ASNI flags

        http://pueblo.sourceforge.net/doc/manual/ansi_color_codes.html
        Code:   Client:     Meaning:
        [0m     --          reset; clears all colors and styles (to white on black)
        [1m     --          bold on (see below)
        [3m     --          italics on
        [4m     --          underline on
        [7m     2.50        inverse on; reverses foreground & background colors
        [9m     2.50        strikethrough on
        [22m    2.50        bold off (see below)
        [23m    2.50        italics off
        [24m    2.50        underline off
        [27m    2.50        inverse off
        [29m    2.50        strikethrough off
        [30m    --          set foreground color to black
        [31m    --          set foreground color to red
        [32m    --          set foreground color to green
        [33m    --          set foreground color to yellow
        [34m    --          set foreground color to blue
        [35m    --          set foreground color to magenta (purple)
        [36m    --          set foreground color to cyan
        [37m    --          set foreground color to white
        [39m    2.53        set foreground color to default (white)
        [40m    --          set background color to black
        [41m    --          set background color to red
        [42m    --          set background color to green
        [43m    --          set background color to yellow
        [44m    --          set background color to blue
        [45m    --          set background color to magenta (purple)
        [46m    --          set background color to cyan
        [47m    --          set background color to white
        [49m    2.53        set background color to default (black)

    */
    struct flags {

        // Common flags
        enum {
            nothing     = 0,        // Nothing changes

            bold_on    = 0x1,   // [1m bold on (see below)
            italics_on = 0x2,   // [3m italics on
            underline_on = 0x4, // [4m underline on
            inverse_on   = 0x8, // [7m inverse on; reverses foreground & background colors
            strikethrough_on = 0x10, // [9m strikethrough on

            _reset = 0x20,     // [0m reset; clears all colors and styles (to white on black)

            bold_off = 0x40,
            italics_off  = 0x80,
            underline_off = 0x100,
            inverse_off = 0x200,
            strikethrough_off = 0x400

            /*
                [22m    2.50        bold off(see below)
                [23m    2.50        italics off
                [24m    2.50        underline off
                [27m    2.50        inverse off
                [29m    2.50        strikethrough off
            */

        };

        enum _foreground{
            //[30m    --          set foreground color to black
            black,
            //[31m    --          set foreground color to red
            red,
            //[32m    --          set foreground color to green
            green,
            //[33m    --          set foreground color to yellow
            yellow,
            //[34m    --          set foreground color to blue
            blue,
            //[35m    --          set foreground color to magenta(purple)
            magenta,
            //[36m    --          set foreground color to cyan
            cyan,
            //[37m    --          set foreground color to white
            white,
            //[39m    2.53        set foreground color to default (white)
            _default,

            undefined
        };
    };

    template<typename container>
    struct color_setter {
    public:
        
        uint8_t _color;

        color_setter() {
            reset();
        }

        virtual container &self() = 0;

        #define COLORS_SET(__color)      \
            container &__color() {       \
                _color = flags::__color; \
                return self();        \
            }

            COLORS_SET(black)
            COLORS_SET(red)
            COLORS_SET(green)
            COLORS_SET(yellow)
            COLORS_SET(blue)
            COLORS_SET(magenta)
            COLORS_SET(cyan)
            COLORS_SET(white)
        #undef COLORS_SET

        void reset() {
            _color = flags::undefined;
        }

        #undef COLORS_SET

        operator uint8_t() {
            return _color;
        }

        container &operator = (uint8_t __color) {
            _color = __color;
            return self();
        }

        const char *get_rep(bool _default = false) {
            static const char *list[] = {
                "30", "31", "32", "33", "34", "35", "36", "37", "39"
            };

            if (_default) {
                return "39";
            }

            if(_color <= flags::_default)
                return list[_color];

            return "39"; // The default one.
        }
    };

    struct bg_color_setter:public color_setter<bg_color_setter> {
        bg_color_setter():color_setter() {}

        virtual bg_color_setter &self() override {
            return *this;
        }

        const char *get_rep(bool _default = false) {
            static const char *list[] = {
                "40", "41", "42", "43", "44", "45", "46", "47", "49"
            };

            if (_default) {
                return "49";
            }

            if (_color <= flags::_default)
                return list[_color];

            return "49"; // The default one.
        }
    };

    struct pallate {
        uint8_t foreground;
        uint8_t background;

        pallate() :
            foreground(flags::undefined), background(flags::undefined) {
            
        }

        pallate(uint8_t fg, uint8_t bg) :
            foreground(fg), background(bg) {

        }

        enum default_pallates {
            none = 0,
            info = 1,
            notice,
            warning,
            error,
            critial,
            fatal
        };
    };

    inline pallate sys_pallate(pallate::default_pallates _pallate, pallate *ptr = nullptr) {
        static pallate pallates[] = {
            pallate(flags::undefined, flags::undefined),      // 0
            pallate(flags::green,     flags::undefined),     // info
            pallate(flags::blue,      flags::undefined),     // notice
            pallate(flags::red,       flags::undefined),     // warning
            pallate(flags::yellow,    flags::undefined),     // error
            pallate(flags::white,     flags::yellow   ),     // critical
            pallate(flags::white,     flags::red      )      // fatal
        };

        if (_pallate > 0 && _pallate <= pallate::fatal) {
            if (ptr) {
                pallates[_pallate] = *ptr;

                return *ptr;
            }
                
            return pallates[_pallate];
        }

        return pallates[0];
    }

    struct color: public color_setter<color>{

        /**
         * Flags to be set.
         */
        uint16_t flag = flags::nothing;

        const char *ref = nullptr;
        bg_color_setter bg;

        color(const char *str = nullptr) {
            ref = str;
        }

        color(std::string &str) {
            ref = str.c_str();
        }

        virtual color &self() override {
            return *this;
        }

        color &set_flag(uint16_t value_on, uint16_t value_off, bool on) {
            if (on) {
                flag |= value_on;
                flag &= ~value_off;
            }
            else {
                flag |= value_off;
                flag &= ~value_on;
            }

            return *this;
        }

        // Flags here.

        #define COLORS_FLAGS_SET(flag) \
            color &flag(bool on = true) { \
                return set_flag(flags::flag##_on, flags::flag##_off, on); \
            }

            COLORS_FLAGS_SET(bold)
            COLORS_FLAGS_SET(italics)
            COLORS_FLAGS_SET(underline)
            COLORS_FLAGS_SET(inverse)
            COLORS_FLAGS_SET(strikethrough)
        #undef COLORS_FLAGS_SET

        #define COLORS_PALLATE(name)                    \
            color &name() {                             \
                pallate p = sys_pallate(pallate::name); \
                bg._color = p.background;               \
                _color = p.foreground;                  \
                return *this;                           \
            }

            COLORS_PALLATE(info)
            COLORS_PALLATE(notice)
            COLORS_PALLATE(warning)
            COLORS_PALLATE(error)
            COLORS_PALLATE(critial)
            COLORS_PALLATE(fatal)

            // TODO: ENHANCEMENT: Add more default plats.
        #undef COLORS_PALLATE

        #define BG_COLORS_SET(__color)      \
            color &bg_##__color() {       \
                bg._color = flags::__color; \
                return self();        \
            }

            BG_COLORS_SET(black)
            BG_COLORS_SET(red)
            BG_COLORS_SET(green)
            BG_COLORS_SET(yellow)
            BG_COLORS_SET(blue)
            BG_COLORS_SET(magenta)
            BG_COLORS_SET(cyan)
            BG_COLORS_SET(white)
        #undef COLORS_SET

        color &reset() {
            flag = flags::nothing;
            color_setter::reset();
            bg.reset();
            return *this;
        }

        color &clear() {
            flag = flags::_reset | 
                flags::bold_off | flags::italics_off |
                flags::underline_off | flags::inverse_off | 
                flags::strikethrough_off;
            _color = flags::_default;
            bg._color = flags::_default;
            return *this;
        }

        bool has_flag(uint16_t test) {
            return (flag & test) == test;
        }

        void has_flag_append(std::string &str, uint16_t _flag, const char *_ansi) {
            if (has_flag(_flag)) {
                str += (char)(27);
                str += _ansi;
            }
        }

        enum color_operation {
            color_enable = 1,
            color_disable = 2,
            color_do_nothing = 3
        };

        bool color_enabled(color_operation enable = color_do_nothing) {
            static bool _color_enabled = (_isatty(_fileno(stdout)) > 0);

            if (enable == color_enable)
                _color_enabled = true;
            else if (enable == color_disable)
                _color_enabled = false;

            return _color_enabled;
        }

        std::string &append_to(std::string &str) {
            if (!color_enabled()) {
                if (ref)
                    return str += ref;

                return str;
            }
                
            if (flag != flags::nothing) {
                if (has_flag(flags::_reset)) {
                    str += (char)(27);
                    str += "[0m";
                    return str;
                }

                //Append all flags,
                has_flag_append(str, flags::bold_on, "[1m");
                has_flag_append(str, flags::italics_on, "[3m");
                has_flag_append(str, flags::underline_on, "[4m");
                has_flag_append(str, flags::inverse_on, "[7m");
                has_flag_append(str, flags::strikethrough_on, "[9m");
                has_flag_append(str, flags::bold_off, "[22m");
                has_flag_append(str, flags::italics_off, "[23m");
                has_flag_append(str, flags::underline_off, "[24m");
                has_flag_append(str, flags::inverse_off, "[27m");
                has_flag_append(str, flags::strikethrough_off, "[29m");
            }

            //Append front and background.
            if (_color != flags::undefined || bg != flags::undefined) {
                str += (char)(27);
                str += '[';
                if (_color != flags::undefined) {
                    str += get_rep();

                    if (bg != flags::undefined)
                        str += ';';
                } 

                if (bg != flags::undefined) {
                    str += bg.get_rep();
                }

                str += 'm';
            }

            // If there are containers, we will close the flags we have opened.
            if (ref) {
                str += ref;

                // Reset the color part
                if (_color != flags::undefined || bg != flags::undefined) {
                    str += (char)(27);
                    str += '[';
                    if (_color != flags::undefined) {
                        str += get_rep(true);

                        if (bg != flags::undefined)
                            str += ';';
                    }

                    if (bg != flags::undefined) {
                        str += bg.get_rep(true);
                    }

                    str += 'm';
                }

                if (flag != flags::nothing) {
                    // reset the flag part
                    has_flag_append(str, flags::bold_on, "[22m");
                    has_flag_append(str, flags::italics_on, "[23m");
                    has_flag_append(str, flags::underline_on, "[24m");
                    has_flag_append(str, flags::inverse_on, "[27m");
                    has_flag_append(str, flags::strikethrough_on, "[29m");
                    has_flag_append(str, flags::bold_off, "[1m");
                    has_flag_append(str, flags::italics_off, "[3m");
                    has_flag_append(str, flags::underline_off, "[4m");
                    has_flag_append(str, flags::inverse_off, "[7m");
                    has_flag_append(str, flags::strikethrough_off, "[9m");
                }
            }

            return str;
        }

        std::string append(std::string &str) {

            std::string cache;
            append_to(cache);
            //Append all flags,
            //Append front and background.

            return cache += str;
        }

        operator std::string() {
            std::string cache;
            append_to(cache);
            return cache;
        }

        std::string str() {
            std::string cache;
            append_to(cache);
            return cache;
        }
    };

    ///////////////////////////////////////////////////////////
    // Frontground colors
    // Access with red(ptr)
    // If the pointer is null, then the system will actually 
    // start the flag and do not close it.

    #define COLOR_FUNC(name)                     \
        color name(const char *_str = nullptr) { \
            return color(_str).name();           \
        }                                        \
        color name(std::string &_str) {          \
            return color(_str).name();           \
        }                    

        COLOR_FUNC(black)
        COLOR_FUNC(red)
        COLOR_FUNC(green)
        COLOR_FUNC(yellow)
        COLOR_FUNC(blue)
        COLOR_FUNC(magenta)
        COLOR_FUNC(cyan)
        COLOR_FUNC(white)

        COLOR_FUNC(bold)
        COLOR_FUNC(italics)
        COLOR_FUNC(underline)
        COLOR_FUNC(inverse)
        COLOR_FUNC(strikethrough)
    #undef COLOR_FUNC

    ///////////////////////////////////////////////////////////
    // Frontground colors
    // Access with red(ptr)
    // If the pointer is null, then the system will actually 
    // start the flag and do not close it.
    struct bg {
    #define BG_COLOR_FUNC(name)                         \
        static color name(const char *_str = nullptr) { \
            return color(_str).name();                  \
        }                                               \
        static color name(std::string &_str) {          \
            return color(_str).name();                  \
        }                    

        BG_COLOR_FUNC(black)
        BG_COLOR_FUNC(red)
        BG_COLOR_FUNC(green)
        BG_COLOR_FUNC(yellow)
        BG_COLOR_FUNC(blue)
        BG_COLOR_FUNC(magenta)
        BG_COLOR_FUNC(cyan)
        BG_COLOR_FUNC(white)
    #undef BG_COLOR_FUNC
    };

    std::string &operator + (std::string &s, color c) {
        return c.append_to(s);
    }

    std::string operator + (color c, std::string &s) {
        return c.append(s);
    }

    std::string &operator << (std::string &s, color c) {
        return c.append_to(s);
    }

    std::string operator << (color c, std::string &s) {
        return c.append(s);
    }

   
}
