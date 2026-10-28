import hatchling.builders.hooks.plugin.interface as hooks

import sysconfig
from typing import Any


class BuildHook(hooks.BuildHookInterface):
    def initialize(self, version: str, build_data: dict[str, Any]) -> None:
        platform = sysconfig.get_platform().replace("-", "_").replace(".", "_")
        build_data["tag"] = f"py3-none-{platform}"
        build_data["pure_python"] = False