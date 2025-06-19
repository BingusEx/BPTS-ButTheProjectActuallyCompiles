#include "FilterFormType.h"

FilterFormType::FilterFormType(RE::FormType formType, bool remove)
{
    FormType = formType;
    Remove = remove;
}

FilterFormType::FilterFormType(std::string formTypeName, bool remove)
{
    FormType = MakeFormType(formTypeName);
    Remove = remove;
}

RE::FormType FilterFormType::MakeFormType(std::string formTypeName)
{
    if (formTypeName == "all") return RE::FormType::Max;
    if (formTypeName == "misc") return RE::FormType::Misc;
    if (formTypeName == "activator") return RE::FormType::Activator;
    if (formTypeName == "actorcharacter") return RE::FormType::ActorCharacter;
    if (formTypeName == "npc") return RE::FormType::NPC;
    if (formTypeName == "container") return RE::FormType::Container;
    if (formTypeName == "ingredient") return RE::FormType::Ingredient;
    if (formTypeName == "flora") return RE::FormType::Flora;
    if (formTypeName == "tree") return RE::FormType::Tree;
    if (formTypeName == "weapon") return RE::FormType::Weapon;
    if (formTypeName == "door") return RE::FormType::Door;
    if (formTypeName == "furniture") return RE::FormType::Furniture;
    if (formTypeName == "alchemyitem") return RE::FormType::AlchemyItem;
    if (formTypeName == "book") return RE::FormType::Book;
    if (formTypeName == "armor") return RE::FormType::Armor;
    if (formTypeName == "ammo") return RE::FormType::Ammo;
    if (formTypeName == "note") return RE::FormType::Note;
    if (formTypeName == "scroll") return RE::FormType::Scroll;
    if (formTypeName == "soulgem") return RE::FormType::SoulGem;
    if (formTypeName == "armor") return RE::FormType::Armor;
    if (formTypeName == "spell") return RE::FormType::Spell;
    if (formTypeName == "keymaster") return RE::FormType::KeyMaster;
    if (formTypeName == "talkingactivator") return RE::FormType::TalkingActivator;
    if (formTypeName == "apparatus") return RE::FormType::Apparatus;
    if (formTypeName == "projectile") return RE::FormType::Projectile;
    if (formTypeName == "light") return RE::FormType::Light;

	return RE::FormType::None;
}
