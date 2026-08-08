#ifndef AWTRIX_DISABLE_TIMER
#include "SyncEnvelope.h"

// The Timer-sync wire envelope + pure inbound receive gate. See SyncEnvelope.h.
// Extracted from TimerManager.

namespace SyncEnvelope
{
    bool targetsMe(JsonVariantConst tgt, const String &ownId)
    {
        // A string tgt is the broadcast spelling: only the literal "all" covers us.
        if (tgt.is<const char *>())
        {
            String s = tgt.as<String>();
            s.trim();
            return s == "all";
        }
        // An array tgt is an explicit id list: membership by uniqueID.
        if (tgt.is<JsonArrayConst>())
        {
            for (JsonVariantConst v : tgt.as<JsonArrayConst>())
            {
                String id = v.as<String>();
                id.trim();
                if (id == ownId) return true;
            }
        }
        return false;
    }

    Decision classify(JsonVariantConst packet, const Context &ctx)
    {
        JsonVariantConst sync = packet["_sync"];
        if (sync.isNull()) return {Decision::Ignore, String(), 0};   // not a sync packet

        String src = sync["src"].as<String>();
        if (src.length() == 0 || src == ctx.ownId)
            return {Decision::Ignore, String(), 0};                  // malformed / own echo

        // Presence beacon: harvest the sender's uniqueID UNGATED —
        // presence is informational, not a command, so it bypasses the follow/target
        // gate. It carries no action/duration/config; short-circuit here.
        if (packet["presence"].as<bool>())
            return {Decision::HarvestPresence, src, 0};

        if (!ctx.follow) return {Decision::Ignore, String(), 0};     // consent gate
        if (!targetsMe(sync["tgt"], ctx.ownId))
            return {Decision::Ignore, String(), 0};                  // not addressed here

        return {Decision::Apply, src, sync["seq"].as<uint32_t>()};
    }

    void build(JsonObject sync, const String &ownId, uint32_t seq)
    {
        sync["src"] = ownId;
        sync["seq"] = seq;
    }

    void build(JsonObject sync, const String &ownId, uint32_t seq, const String &targets)
    {
        build(sync, ownId, seq);

        String t = targets;
        t.trim();
        if (t == "all") { sync["tgt"] = "all"; return; }

        JsonArray arr = sync.createNestedArray("tgt");
        int start = 0;
        const int n = t.length();
        while (start < n)
        {
            int comma = t.indexOf(',', start);
            if (comma < 0) comma = n;
            String id = t.substring(start, comma);
            id.trim();
            if (id.length() > 0) arr.add(id);
            start = comma + 1;
        }
    }
}
#endif // AWTRIX_DISABLE_TIMER
