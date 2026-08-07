# rigComponent

![preview](examples/creators/img/preview.png)

**Data-only** ECS pack for RigKit

Headers live under `src/` (`C*.h`).

## Build Example

```bash
cmake -S examples/creators -B examples/creators/build
cmake --build examples/creators/build --target creators
```

**Bootstrap order**: register `rigComponent` before `rigSystems` / `UI packs`.

[API/docs](https://rigkid.github.io/rigComponent/)
