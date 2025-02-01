int iscntrl(int c) {
    char ch = (char)c;
    return (ch >= 0 && ch <= 31) || (ch == 127);
}