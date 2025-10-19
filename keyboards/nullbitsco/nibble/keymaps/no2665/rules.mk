OLED_ENABLE = yes
OLED_DRIVER = ssd1306
OLED_TRANSPORT = i2c
WPM_ENABLE = yes

SRC += 	oled.c \
		oled_animations/matrix.c \
		oled_animations/layer_icons.c \
		oled_animations/startup.c \
		oled_animations/stats.c
