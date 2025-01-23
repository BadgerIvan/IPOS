# IPOS

IPOS - это небольшая операционная система, создаваемая с нуля. Проект предназначен для обучения, экспериментов и исследования принципов работы операционных систем.

## Основные возможности

- **Ядро:** описание ядра и его базовая функциональность.

## Планируемые возможности

- **Управление процессами:** как операционная система управляет процессами, включая планирование задач.
- **Управление памятью:** работа с памятью, включая распределение и освобождение ресурсов.
- **Ввод/вывод:** обработка устройств ввода и вывода.
- **Файловая система:** реализация базовой файловой системы (если она есть).

## Требования

- **Компилятор:** GCC для i386
- **Сборка:** make
- **Эмулятор:** рекомендуется использовать qemu

За помощью с установкой инструментов можете обратиться к [wiki.osdev.org](https://wiki.osdev.org/GCC_Cross-Compiler).

## Установка и запуск

1. Клонируйте репозиторий:
   ```bash
   git clone https://github.com/BadgerIvan/IPOS.git
   cd IPOS
   ```
2. Соберите проект:
   ```bash
   ./iso.sh
   ```
3. Запустите в эмуляторе:
   ```bash
   ./qemu.sh
   ```

## Вклад в проект

Если вы хотите внести свой вклад:

1. Форкните репозиторий.
2. Создайте новую ветку для ваших изменений.
3. Сделайте коммит и отправьте изменения.
4. Откройте pull request.

## Лицензия

Проект распространяется под лицензией MIT. См. файл `LICENSE` для получения дополнительной информации.