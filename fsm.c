#include <stdio.h>

typedef enum
{
    OFF,
    ON,
    NUM_STATES
} state_variable;

typedef enum
{
    BUTTON_PUSH,
    NUM_EVENTS
} event_code;

typedef void (*handler_ref)(event_code);

typedef struct
{
    event_code evt_code;
    handler_ref handler;
    state_variable next_state;
} tt_entry;

void state_off(event_code evt)
{
    switch (evt)
    {
        case BUTTON_PUSH:
            puts("state_off");
            break;
    }
}

void state_on(event_code evt)
{
    switch (evt)
    {
        case BUTTON_PUSH:
            puts("state_on");
            break;
    }
}

const static tt_entry tt_button[NUM_STATES][NUM_EVENTS] =
{
    [OFF][BUTTON_PUSH] = {BUTTON_PUSH, state_off, ON},
    [ON][BUTTON_PUSH] = {BUTTON_PUSH, state_on, OFF}
};

typedef struct
{
    state_variable current_state;
} context;


void handle_event(const tt_entry table[NUM_STATES][NUM_EVENTS], event_code evt, context *ctx)
{
    const tt_entry *entry = &table[ctx->current_state][evt];

    if (entry->handler)    
    {
        entry->handler(evt);
        ctx->current_state = entry->next_state;
    }
    else
    {
        printf("No handler for event %d in state %d\n", evt, ctx->current_state);
    }
}

int main()
{
    context ctx = {.current_state = OFF};
    handle_event(tt_button, BUTTON_PUSH, &ctx);
    handle_event(tt_button, BUTTON_PUSH, &ctx);
    handle_event(tt_button, BUTTON_PUSH, &ctx);
}