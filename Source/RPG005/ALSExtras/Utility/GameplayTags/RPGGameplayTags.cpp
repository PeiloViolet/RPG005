#include "RPGGameplayTags.h"

namespace StatisticTags
{
	UE_DEFINE_GAMEPLAY_TAG(Health, TEXT("RPG.Statistic.Health"))
	UE_DEFINE_GAMEPLAY_TAG(Mana, TEXT("RPG.Statistic.Mana"))
	UE_DEFINE_GAMEPLAY_TAG(Stamina, TEXT("RPG.Statistic.Stamina"))
	UE_DEFINE_GAMEPLAY_TAG(Equilibrium, TEXT("RPG.Statistic.Equilibrium"))
}

namespace AttributeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Vigor, TEXT("RPG.Attribute.Vigor"))
	UE_DEFINE_GAMEPLAY_TAG(Mind, TEXT("RPG.Attribute.Mind"))
	UE_DEFINE_GAMEPLAY_TAG(Endurance, TEXT("RPG.Attribute.Endurance"))
	UE_DEFINE_GAMEPLAY_TAG(Strength, TEXT("RPG.Attribute.Strength"))
	UE_DEFINE_GAMEPLAY_TAG(Dexterity, TEXT("RPG.Attribute.Dexterity"))
	UE_DEFINE_GAMEPLAY_TAG(Intelligence, TEXT("RPG.Attribute.Intelligence"))
	UE_DEFINE_GAMEPLAY_TAG(Faith, TEXT("RPG.Attribute.Faith"))
	UE_DEFINE_GAMEPLAY_TAG(Arcane, TEXT("RPG.Attribute.Arcane"))
	UE_DEFINE_GAMEPLAY_TAG(Resistence, TEXT("RPG.Attribute.Resistence"))
}

namespace ParametersTags
{
	UE_DEFINE_GAMEPLAY_TAG(MovementSpeed, TEXT("RPG.Parameters.MovementSpeed"))
	UE_DEFINE_GAMEPLAY_TAG(ActionSpeed, TEXT("RPG.Parameters.ActionSpeed"))
	UE_DEFINE_GAMEPLAY_TAG(MeleeDamage, TEXT("RPG.Parameters.Damage.Melee"))
	UE_DEFINE_GAMEPLAY_TAG(RangedDamage, TEXT("RPG.Parameters.Damage.Ranged"))
	UE_DEFINE_GAMEPLAY_TAG(MagicDamage, TEXT("RPG.Parameters.Damage.Magic"))
	UE_DEFINE_GAMEPLAY_TAG(CritChance, TEXT("RPG.Parameters.CritChance"))
	UE_DEFINE_GAMEPLAY_TAG(CritDamage, TEXT("RPG.Parameters.CritDamage"))
	UE_DEFINE_GAMEPLAY_TAG(VampireRate, TEXT("RPG.Parameters.VampireRate"))
	UE_DEFINE_GAMEPLAY_TAG(Impact, TEXT("RPG.Parameters.Impact"))
	UE_DEFINE_GAMEPLAY_TAG(Penetration, TEXT("RPG.Parameters.Penetration"))
	UE_DEFINE_GAMEPLAY_TAG(FreezeStatus, TEXT("RPG.Parameters.Status.Freeze"))
	UE_DEFINE_GAMEPLAY_TAG(BleedStatus, TEXT("RPG.Parameters.Status.Bleed"))
	UE_DEFINE_GAMEPLAY_TAG(PoisoningStatus, TEXT("RPG.Parameters.Status.Poisoning"))
	UE_DEFINE_GAMEPLAY_TAG(VertigoStatus, TEXT("RPG.Parameters.Status.Vertigo"))
	UE_DEFINE_GAMEPLAY_TAG(CorruptionStatus, TEXT("RPG.Parameters.Status.Corruption"))
	UE_DEFINE_GAMEPLAY_TAG(EroticismStatus, TEXT("RPG.Parameters.Status.Eroticism"))
	UE_DEFINE_GAMEPLAY_TAG(ChaosStatus, TEXT("RPG.Parameters.Status.Chaos"))
	UE_DEFINE_GAMEPLAY_TAG(StrikePhysicalDefense, TEXT("RPG.Parameters.PhysicalDefense.Strike"))
	UE_DEFINE_GAMEPLAY_TAG(SlashPhysicalDefense, TEXT("RPG.Parameters.PhysicalDefense.Slash"))
	UE_DEFINE_GAMEPLAY_TAG(PiercePhysicalDefense, TEXT("RPG.Parameters.PhysicalDefense.Pierce"))
	UE_DEFINE_GAMEPLAY_TAG(FireMagicalDefense, TEXT("RPG.Parameters.MagicalDefense.Fire"))
	UE_DEFINE_GAMEPLAY_TAG(IceMagicalDefense, TEXT("RPG.Parameters.MagicalDefense.Ice"))
	UE_DEFINE_GAMEPLAY_TAG(ThunderMagicalDefense, TEXT("RPG.Parameters.MagicalDefense.Thunder"))
	UE_DEFINE_GAMEPLAY_TAG(HolyMagicalDefense, TEXT("RPG.Parameters.MagicalDefense.Holy"))
	UE_DEFINE_GAMEPLAY_TAG(StatusDefense, TEXT("RPG.Parameters.StatusDefense"))
	UE_DEFINE_GAMEPLAY_TAG(DodgeChance, TEXT("RPG.Parameters.DodgeChance"))
	UE_DEFINE_GAMEPLAY_TAG(BlockDefense, TEXT("RPG.Parameters.BlockDefense"))
}
