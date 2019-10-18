package extractor.transition;

import global.structure.State;
import global.structure.Transition;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.ArrayList;
import java.util.List;

public class Iterator {
    //Attributes
    private TransitionTools tools;
    private boolean breakFlag;
    private int scopeLevel;
    private State state;

    //Constructor
    public Iterator (State state, int scopeLevel) {
        tools = new TransitionTools();
        breakFlag = false;
        this.scopeLevel = scopeLevel;
        this.state = state;
    }

    //Methods
    public List<Transition> iterate (List<Transition> response, List<State> states, List<Transition> transitions, List<State> functions) {
        List<Transition> newResponse;
        do {
            newResponse = (process (response, states, transitions, functions));
            response = newResponse;
            if (breakFlag) {
                break;
            }
        } while (state.getScopeLevel() > scopeLevel);
        return newResponse;
    }

    private List<Transition> process (List<Transition> response, List<State> states, List<Transition> transitions, List<State> functions) {
        List<Transition> newResponse = new ArrayList<>();
        switch (state.getElement()) {
            case JUMP:
                if (state.getLabel().equals("break")) {
                    breakFlag = true;
                    return response;
                }
                if (state.getLabel().equals("return")) {
                    tools.finalizeTransition(response, state, transitions);
                    tools.finalizeTransition(functionCall(response, states, transitions, functions), state, transitions);
                    newResponse.add(tools.createTransition(extractReturnEvent(state), state, null));
                    breakFlag = true;
                    break;
                }
            case ATTRIBUTION:
            case STATEMENT:
                tools.finalizeTransition(response, state, transitions);
                tools.finalizeTransition(functionCall(response, states, transitions, functions), state, transitions);
                String event = tools.extractEvent(state.getLine());
                newResponse.add(tools.createTransition(event, state, null));
                state = tools.getNextState(state, states);
                break;
            case DECISION:
                tools.finalizeTransition(response, state, transitions);
                tools.finalizeTransition(functionCall(response, states, transitions, functions), state, transitions);
                if (state.getLabel().equals("if")) {
                    while (true) {
                        List<Transition> falseResponse = new ArrayList<>();
                        falseResponse.add(tools.createTransition("FALSE", state, null));
                        newResponse.addAll(iterateLoop(states, transitions, functions));
                        state = tools.getNextSameLevelState(state, states);
                        if (!state.getLabel().equals("else_if") && !state.getLabel().equals("else")) {
                            newResponse.addAll(falseResponse);
                            break;
                        } else {
                            tools.finalizeTransition(falseResponse, state, transitions);
                        }
                    }
                } else {
                    state = tools.getNextState(state, states);
                    return response;
                }
                break;
            case LOOP:
                tools.finalizeTransition(response, state, transitions);
                tools.finalizeTransition(functionCall(response, states, transitions, functions), state, transitions);
                if (state.getLabel().equals("do")) {
                    tools.finalizeTransition(iterateLoop(states, transitions, functions), tools.getNextSameLevelState(state, states), transitions);
                    transitions.add(tools.createTransition("FALSE", tools.getNextSameLevelState(state, states), state));
                    newResponse.add(tools.createTransition("TRUE", tools.getNextSameLevelState(state, states), null));
                    state = tools.getNextState(tools.getNextSameLevelState(state, states), states);
                    break;
                }
                if (state.getLabel().equals("while")) {
                    newResponse.add(tools.createTransition("FALSE", state, null));
                    newResponse.addAll(iterateLoop(states, transitions, functions));
                    tools.finalizeTransition(iterateLoop(states, transitions, functions), state, transitions);
                    state = tools.getNextSameLevelState(state, states);
                    break;
                }
                if (state.getLabel().equals("for")) {
                    newResponse.add(tools.createTransition("FALSE", state, null));
                    newResponse.addAll(iterateLoop(states, transitions, functions));
                    tools.finalizeTransition(iterateLoop(states, transitions, functions), state, transitions);
                    state = tools.getNextSameLevelState(state, states);
                    break;
                }
                if (state.getLabel().equals("switch")) {
                    State me = state;
                    state = tools.getNextState(state, states);
                    if (!imInside(me)) return response;
                    List<Transition> innerResponse = new ArrayList<>();
                    while (imInside(me)) {
                        while (state.getLabel().equals("case")) {
                            innerResponse.add(tools.createTransition(extractReturnEvent(state), me, null));
                            state = tools.getNextState(state, states);
                        }
                        if (state.getLabel().equals("default")) {
                            innerResponse.add(tools.createTransition("lambda", me, null));
                            state = tools.getNextState(state, states);
                        } else {
                            Iterator iterator = new Iterator(state, me.getScopeLevel());
                            newResponse.addAll(iterator.iterate(innerResponse, states, transitions, functions));
                            innerResponse.clear();
                            state = tools.getNextSameLevelDecision(state, states);
                        }
                    }
                    break;
                }
            default:
                state = tools.getNextState(state, states);
                return response;
        }
        return newResponse;
    }

    private List<Transition> functionCall (List<Transition> response, List<State> states, List<Transition> transitions, List<State> functions) {
        List<Transition> newResponse = new ArrayList<>();
        for (ParseTree key : state.getLine().getContent()) {
            for (State function : functions) {
                if (function.getLabel().equals(key.getText())) {
                    Iterator iterator = new Iterator(function, function.getScopeLevel());
                    newResponse.addAll(iterator.iterate(response, states, transitions, functions));
                }
            }
        }
        return newResponse;
    }

    private boolean imInside (State me) {
        return state.getScopeLevel() == me.getScopeLevel() + 1;
    }

    private String extractReturnEvent(State state) {
        try {
            return state.getLine().getContent().get(1).getText();
        } catch (Exception e) {
            return "";
        }
    }

    private List<Transition> iterateLoop (List<State> states, List<Transition> transitions, List<State> functions) {
        List<Transition> trueResponse = new ArrayList<>();
        trueResponse.add(tools.createTransition("TRUE", state, null));
        Iterator iterator = new Iterator(tools.getNextState(state, states), state.getScopeLevel());
        return (iterator.iterate(trueResponse, states, transitions, functions));
    }

}
