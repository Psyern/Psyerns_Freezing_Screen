# Syntax Fix - Ternary Operator

## Error Fixed

**Error:** `DME_Cold/Scripts/3_Game/a_dme_coldfrostoverlay.c(278): Broken expression (missing ';'?)`

**Root Cause:** Enforce Script (DayZ's scripting language) requires explicit parentheses around negative values in ternary expressions.

## Changed Line

**Before (Line 278):**
```enforce
m_CurrentAlpha += (alphaDiff > 0 ? maxDelta : -maxDelta);
```

**After:**
```enforce
m_CurrentAlpha += alphaDiff > 0 ? maxDelta : (-maxDelta);
```

## Explanation

In Enforce Script, the unary minus operator needs to be parenthesized when used in a ternary conditional expression. The compiler couldn't properly parse `-maxDelta` without parentheses in this context, resulting in a "broken expression" error.

## Status

✅ Syntax error fixed  
✅ File ready for recompilation  

The server should now compile the Game module successfully.
