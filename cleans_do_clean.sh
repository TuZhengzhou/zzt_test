find ./cpp/ -type f \( -name "*.o" -o -executable  ! -name "*.sh" \) | xargs -r rm -v 2>/dev/null
find ./daybyday/ -type f \( -name "*.o" -o -executable ! -name "*.sh" \) | xargs -r rm -v 2>/dev/null