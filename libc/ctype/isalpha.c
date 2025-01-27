int isalpha(int c) {
    char ch = (char)c;
    return ((ch >= 'A' && ch <= 'Z') ||
            (ch >= 'a' && ch <= 'z'));
}