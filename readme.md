# Compilation
Il faut commenter la fonction de la ligne 37 dans le fichier `external/imgui-node-editor/imgui-node-editor/imgui_extra_math.inl`
```c++
inline static ImVec2 operator-(const ImVec2& lhs)
{
    return ImVec2(-lhs.x, -lhs.y);
}
```

Sous Linux
```bash
cd build
cmake ..
make
```

