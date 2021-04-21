#include <array>
#include <cstdint>
#include <optional>

class Keypad {
    public:
        void key_up(uint8_t keycode) { m_keysdown[keycode % 16] = false; };
        void key_down(uint8_t keycode) { 
            m_keysdown[keycode % 16] = true; 
            m_keydown_event = keycode % 16;
        };
        bool is_key_down(uint8_t keycode) { return m_keysdown[keycode % 16]; };

        void clear_keydown_event() { m_keydown_event.reset(); };
        bool has_keydown_event() { return m_keydown_event.has_value(); };
        uint8_t get_keydown_event() { return *m_keydown_event; };

    private: 
        std::array<bool, 16> m_keysdown;
        std::optional<uint8_t> m_keydown_event;
};