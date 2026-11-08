"""Prepare a patched ChallengeModes.cpp in an isolated output directory. Never edits the core."""
import argparse
import hashlib
import json
from pathlib import Path


def integrate(text):
    if '#include "Survival.h"' in text:
        raise ValueError("This source is already integrated; start from an unmodified copy.")
    replacements = [
        ('#include "ChallengeModes.h"', '#include "ChallengeModes.h"\n#include "Survival.h"'),
        ('        return IsMartyrChallengeActive(player);',
         '        return SylvaniaSurvival::IsParticipant(player) || IsMartyrChallengeActive(player);'),
        ('            case 100:                        return "Martyr";',
         '            case 100:                        return "Martyr";\n'
         '            case SylvaniaSurvival::ActivationAction: return "Survival";'),
        ('        SendGossipMenuFor(player, 12669, go->GetGUID());',
         '        SylvaniaSurvival::AddAltarOption(player);\n'
         '        SendGossipMenuFor(player, 12669, go->GetGUID());'),
        ('    bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 action) override\n    {',
         '    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action) override\n    {\n'
         '        if (action == SylvaniaSurvival::ActivationAction)\n'
         '        {\n'
         '            if (SylvaniaSurvival::ActivateAtAltar(player, go))\n'
         '                AnnounceChallengeActivation(player, action);\n'
         '            CloseGossipMenuFor(player);\n'
         '            return true;\n'
         '        }'),
    ]
    for old, new in replacements:
        if text.count(old) != 1:
            raise ValueError(f"Integration anchor changed or ambiguous: {old[:85]!r}")
        text = text.replace(old, new, 1)
    # The public header exports the action as well as the bridge functions.
    return text


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.source.resolve() == args.output.resolve():
        parser.error("Output must differ from the source; this tool only prepares a review copy.")
    original = args.source.read_text(encoding="utf-8")
    updated = integrate(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(updated, encoding="utf-8", newline="\n")
    print(json.dumps({"source": str(args.source), "output": str(args.output),
                      "source_sha256": hashlib.sha256(original.encode()).hexdigest(),
                      "prepared_sha256": hashlib.sha256(updated.encode()).hexdigest(), "changes": 5}))


if __name__ == "__main__":
    main()
