int ispunct(int c) {
    char ch = (char)c;
    return  ch == '.' ||
            ch == ',' ||
            ch == ';' ||
            ch == ':' ||
            ch == '!' ||
            ch == '?' ||
            ch == '-' ||
            ch == '(' ||
            ch == ')' ||
            ch == '\"' ||
            ch == '\'';
}