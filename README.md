# Knight Fight!? - Game 2D Sử Dụng SDL2

[English Version Below](#knight-fight---2d-game-using-sdl2)

## Giới Thiệu

Knight Fight!? là một trò chơi 2D hành động kết hợp yếu tố chiến đấu và phản xạ. Người chơi sẽ điều khiển một hiệp sĩ đấu với các kẻ địch, sử dụng đòn tấn công, né và phản đòn để sống sót và đánh bại càng nhiều kẻ địch càng tốt.

## Yêu Cầu Hệ Thống

- Hệ điều hành: Windows
- Thư viện SDL2 và các extension:
  - SDL2
  - SDL2_image
  - SDL2_mixer
  - SDL2_ttf

## Cách Cài Đặt

1. Tải về tất cả các file từ repository
2. Đảm bảo các thư viện SDL2 đã được cài đặt
3. Chạy file "Knight fight.exe" trong thư mục chính hoặc biên dịch lại từ mã nguồn

## Cách Chơi

### Điều Khiển
- **W**: Nhảy
- **A/D**: Di chuyển trái/phải
- **Chuột trái**: Tấn công
- **Chuột phải**: Phản đòn
- **Phím Space**: Lướt (chỉ sử dụng được sau khi đã tiêu diệt 15 kẻ địch)
- **ESC**: Tạm dừng trò chơi
- **F1**: Hiển thị hướng dẫn
- **F5**: Khởi động lại khi nhân vật chết

### Cơ Chế Trò Chơi
- **Tấn công**: Gây sát thương cho kẻ địch trong phạm vi
- **Phản đòn**: Nếu sử dụng đúng thời điểm, có thể phản lại đòn tấn công của kẻ địch
- **Lướt**: Kỹ năng đặc biệt, giúp di chuyển nhanh và tránh sát thương; mở khóa sau khi tiêu diệt 15 kẻ địch
- **Thời gian hồi chiêu**: Mỗi kỹ năng có thời gian hồi chiêu, được hiển thị ở góc phải màn hình
- **Mục tiêu**: Tiêu diệt được càng nhiều kẻ địch càng tốt trước khi hết thời gian

## Đặc Điểm Nổi Bật

- Hệ thống chiến đấu đơn giản nhưng đầy thử thách
- Hoạt ảnh đẹp mắt và hiệu ứng âm thanh sống động
- Cơ chế phản đòn và lướt độc đáo
- Thời gian hồi chiêu của kỹ năng lướt giảm dần khi tiêu diệt nhiều kẻ địch
- Hiệu ứng đặc biệt khi đạt được các mốc thành tích

## Cấu Trúc Mã Nguồn

- **main.cpp**: Điểm vào của chương trình, quản lý vòng lặp game
- **Game.cpp/hpp**: Xử lý trạng thái trò chơi, cập nhật và render
- **GameObject.cpp/h**: Định nghĩa đối tượng người chơi và các thuộc tính
- **Enemy.cpp/h**: Định nghĩa đối tượng kẻ địch và AI
- **Combat.cpp/hpp**: Xử lý cơ chế chiến đấu
- **Physics.cpp/hpp**: Quản lý vật lý và va chạm
- **AudioManager.cpp/h**: Quản lý âm thanh và nhạc nền
- **TextureManager.cpp/h**: Quản lý tải và render texture
- **TileMap.cpp/hpp**: Xử lý bản đồ và các tile

## Lưu Ý

- Trò chơi này được phát triển với mục đích giáo dục và giải trí
- Một số tài nguyên âm thanh và hình ảnh được lấy từ bên ngoài, mọi quyền sở hữu thuộc về các tác giả gốc

## Phiên Bản

Phiên bản hiện tại: 1.0

## Credits

- Developed by: Bui Son
- Framework: SDL2
- Âm thanh và hình ảnh: Nguồn mở và được sửa đổi để phù hợp với trò chơi

---

# Knight Fight!? - 2D Game Using SDL2

## Introduction

Knight Fight!? is a 2D action game combining combat and reflex elements. Players control a knight fighting against enemies, using attacks, dodges, and parries to survive and defeat as many enemies as possible.

## System Requirements

- Operating System: Windows
- SDL2 Libraries and extensions:
  - SDL2
  - SDL2_image
  - SDL2_mixer
  - SDL2_ttf

## Installation

1. Download all files from the repository
2. Ensure SDL2 libraries are installed
3. Run "Knight fight.exe" in the main folder or compile from source

## How to Play

### Controls
- **W**: Jump
- **A/D**: Move left/right
- **Left Mouse**: Attack
- **Right Mouse**: Parry
- **Space**: Dash (only available after defeating 15 enemies)
- **ESC**: Pause game
- **F1**: Display instructions
- **F5**: Restart when character dies

### Game Mechanics
- **Attack**: Deals damage to enemies within range
- **Parry**: If used at the right moment, can counter enemy attacks
- **Dash**: Special ability that allows quick movement and damage avoidance; unlocked after defeating 15 enemies
- **Cooldowns**: Each ability has a cooldown period, displayed at the top right corner of the screen
- **Objective**: Defeat as many enemies as possible before time runs out

## Key Features

- Simple yet challenging combat system
- Beautiful animations and vibrant sound effects
- Unique parry and dash mechanics
- Dash cooldown reduces as you defeat more enemies
- Special effects when achieving milestones

## Code Structure

- **main.cpp**: Entry point of the program, manages the game loop
- **Game.cpp/hpp**: Handles game state, updates, and rendering
- **GameObject.cpp/h**: Defines player object and properties
- **Enemy.cpp/h**: Defines enemy objects and AI
- **Combat.cpp/hpp**: Handles combat mechanics
- **Physics.cpp/hpp**: Manages physics and collisions
- **AudioManager.cpp/h**: Handles sound and music
- **TextureManager.cpp/h**: Manages texture loading and rendering
- **TileMap.cpp/hpp**: Handles map and tiles

## Notes

- This game was developed for educational and entertainment purposes
- Some audio and visual resources are from external sources, all rights belong to their original creators

## Version

Current version: 1.0

## Credits

- Developed by: Bui Son
- Framework: SDL2
- Audio and visuals: Open source and modified for the game