int isspace(int c) {
    char ch = (char)c;
    return  ch == ' ' || 
            ch == '\t' ||
            ch == '\n' ||
            ch == '\v' ||
            ch == '\f'; 
}