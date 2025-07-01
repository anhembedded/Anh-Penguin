# 📌 **File Descriptor áp dụng cho loại file nào?**

**Không chỉ `/dev` đâu nha!** File Descriptor được dùng cho **mọi loại file hoặc tài nguyên I/O** mà tiến trình mở ra — bao gồm:

| Loại tài nguyên     | Có dùng FD không? | Ví dụ cụ thể                            |
| ---------------------- | -------------------- | ------------------------------------------- |
| 📄 File thường       | ✅ Có               | `.txt`,`.cpp`,`.jpg`, v.v.            |
| 🧪 Thiết bị `/dev` | ✅ Có               | `/dev/ttyS0`,`/dev/null`,`/dev/i2c-1` |
| 🔌 Socket mạng        | ✅ Có               | TCP/UDP socket                              |
| 🔁 Pipe & FIFO         | ✅ Có               | `\|`,`mkfifo`,`pipe()`                 |
| 🧵 Pseudo-terminal     | ✅ Có               | `/dev/pts/0`                              |
| 📦 Block device        | ✅ Có               | `/dev/sda`,`/dev/mmcblk0`               |

---

### 🧠 Vậy tại sao trong Embedded Linux hay nhắc đến `/dev`?

Vì trong hệ thống nhúng:

* Giao tiếp với phần cứng (GPIO, UART, I2C, SPI...) thường thông qua **file trong `/dev`**
* Các thiết bị này được ánh xạ thành **character device** hoặc  **block device** , và bạn thao tác với chúng bằng FD

Nhưng điều đó  **không có nghĩa là FD chỉ dùng cho `/dev`** . Khi bạn mở một file `.txt` bằng `open()` trong C, bạn cũng nhận được một FD y chang như khi mở `/dev/ttyS0`.

---

### 💡 Ví dụ thực tế:

```c
int fd1 = open("hello.txt", O_RDONLY);     // FD cho file thường
int fd2 = open("/dev/ttyS0", O_RDWR);      // FD cho thiết bị serial
```

Cả hai đều là FD hợp lệ, và bạn có thể `read()`, `write()`, `close()` như nhau.

---
