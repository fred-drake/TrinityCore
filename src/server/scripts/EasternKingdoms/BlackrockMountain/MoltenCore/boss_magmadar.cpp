/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Boss_Magmadar
SD%Complete: 75
SDComment: Conflag on ground nyi
SDCategory: Molten Core
EndScriptData */

#include "ScriptMgr.h"
#include "molten_core.h"
#include "ScriptedCreature.h"

enum Texts
{
    EMOTE_FRENZY        = 0
};

enum Spells
{
    SPELL_FRENZY        = 19451,
    SPELL_MAGMA_SPIT    = 19449,
    SPELL_PANIC         = 19408,
    SPELL_LAVA_BOMB     = 19428,
};

enum Events
{
    EVENT_FRENZY        = 1,
    EVENT_PANIC         = 2,
    EVENT_LAVA_BOMB     = 3,
};

class boss_magmadar : public CreatureScript
{
    public:
        boss_magmadar() : CreatureScript("boss_magmadar") { }

        struct boss_magmadarAI : public BossAI
        {
            boss_magmadarAI(Creature* creature) : BossAI(creature, BOSS_MAGMADAR)
            {
            }

            void Reset() override
            {
                BossAI::Reset();
                DoCast(me, SPELL_MAGMA_SPIT, true);
            }

            void JustEngagedWith(Unit* victim) override
            {
                BossAI::JustEngagedWith(victim);
                events.ScheduleEvent(EVENT_FRENZY, 30s);
                events.ScheduleEvent(EVENT_PANIC, 20s);
                events.ScheduleEvent(EVENT_LAVA_BOMB, 12s);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FRENZY:
                            Talk(EMOTE_FRENZY);
                            DoCast(me, SPELL_FRENZY);
                            events.ScheduleEvent(EVENT_FRENZY, 15s);
                            break;
                        case EVENT_PANIC:
                            DoCastVictim(SPELL_PANIC);
                            events.ScheduleEvent(EVENT_PANIC, 35s);
                            break;
                        case EVENT_LAVA_BOMB:
                            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true, true, -SPELL_LAVA_BOMB))
                                DoCast(target, SPELL_LAVA_BOMB);
                            events.ScheduleEvent(EVENT_LAVA_BOMB, 12s);
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetMoltenCoreAI<boss_magmadarAI>(creature);
        }
};

void AddSC_boss_magmadar()
{
    new boss_magmadar();
}
