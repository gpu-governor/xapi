# 2D graphics API:  

### **Core Rendering Functions**  
- **Initialize and Terminate**  
  - `init(width, height, title)`  
  - `destroy(context)`  

- **Rendering Pipeline**  
  - `clear_screen(color)`  
  - `set_color(ctx, color)`  

### **Drawing Primitives**    
- `draw_line(x1, y1, x2, y2, color)`  
- `draw_rectangle(x, y, width, height, color)`  
- `draw_circle(x, y, radius, color)`  
- `draw_triangle(x1, y1, x2, y2, x3, y3, color)`  

### **Sprite Handling**  
- `load_texture(file_path)`  
- `unload_texture(texture)`  
- `draw_texture(texture, x, y)`  
- `draw_texture_ex(texture, x, y, rotation, scale, color_tint)`  

### **Text Rendering**  

- `draw_text(font, text, x, y, size, color)`  

### **Input Handling**  
- `is_key_pressed(key)`  
- `is_key_down(key)`  
- `get_mouse_position()`  

### **Timing and Framerate Control**  
- `set_target_fps(fps)`  
- `get_frame_time()`  
