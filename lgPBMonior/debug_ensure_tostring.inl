#ifdef _WIN32
std::string to_string(DWORD n)
{
    char buf[20];
    snprintf(buf, 20, "%u", n);
    return buf;
}
#endif