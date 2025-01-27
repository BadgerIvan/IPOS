int iscntrl(int c) {
    char ch = (char)c;
    return (c >= 0 && c <= 31) || (c == 127);
}