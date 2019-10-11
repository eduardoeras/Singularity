package extractor.transition;

import global.structure.Element;
import global.structure.Event;
import global.structure.State;
import global.structure.Transition;

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
    public List<Transition> iterate (List<Transition> response, List<State> states, List<Transition> transitions) {
        List<Transition> newResponse;
        do {
            newResponse = (process (response, states, transitions));
            response = newResponse;
            if (breakFlag) {
                break;
            }
        } while (state.getScopeLevel() > scopeLevel);
        return newResponse;
    }

    private List<Transition> process (List<Transition> response, List<State> states, List<Transition> transitions) {
        List<Transition> newResponse = new ArrayList<>();
        switch (state.getElement()) {
            case JUMP:
                if (state.getLabel().equals("break")) {
                    breakFlag = true;
                    return response;
                }
                if (state.getLabel().equals("return")) {
                    createTransition(response, state, transitions);
                    newResponse.add(tools.createTransition(extractEvent(state), state, null));
                    breakFlag = true;
                    break;
                }
            case ATTRIBUTION:
            case STATEMENT:
                createTransition(response, state, transitions);
                String event = tools.extractEvent(state.getLine());
                newResponse.add(tools.createTransition(event, state, null));
                state = tools.getNextState(state, states);
                break;
            case DECISION:
                createTransition(response, state, transitions);
                if (state.getLabel().equals("if")) {
                    while (true) {
                        List<Transition> falseResponse = new ArrayList<>();
                        falseResponse.add(tools.createTransition("FALSE", state, null));
                        newResponse.addAll(iterateLoop(states, transitions));
                        state = tools.getNextSameLevelState(state, states);
                        if (!state.getLabel().equals("else_if") && !state.getLabel().equals("else")) {
                            newResponse.addAll(falseResponse);
                            break;
                        } else {
                            createTransition(falseResponse, state, transitions);
                        }
                    }
                } else {
                    state = tools.getNextState(state, states);
                    return response;
                }
                break;
            case LOOP:
                createTransition(response, state, transitions);
                if (state.getLabel().equals("for")) {
                    newResponse.add(tools.createTransition("FALSE", state, null));
                    newResponse.addAll(iterateLoop(states, transitions));
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
                            innerResponse.add(tools.createTransition(extractEvent(state), me, null));
                            state = tools.getNextState(state, states);
                        }
                        if (state.getLabel().equals("default")) {
                            innerResponse.add(tools.createTransition("lambda", me, null));
                            state = tools.getNextState(state, states);
                        } else {
                            Iterator iterator = new Iterator(state, me.getScopeLevel());
                            newResponse.addAll(iterator.iterate(innerResponse, states, transitions));
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

    private boolean imInside (State me) {
        return state.getScopeLevel() == me.getScopeLevel() + 1;
    }

    private String extractEvent(State state) {
        try {
            return state.getLine().getContent().get(1).getText();
        } catch (Exception e) {
            return "";
        }
    }

    private List<Transition> iterateLoop (List<State> states, List<Transition> transitions) {
        List<Transition> trueResponse = new ArrayList<>();
        trueResponse.add(tools.createTransition("TRUE", state, null));
        Iterator iterator = new Iterator(tools.getNextState(state, states), state.getScopeLevel());
        return (iterator.iterate(trueResponse, states, transitions));
    }

    private void createTransition (List<Transition> response, State state, List<Transition> transitions) {
        for (Transition transition : response) {
            transition.setTo(state);
            if (!existis(transition, transitions)) {
                transitions.add(transition);
            }
        }
    }

    private boolean existis(Transition neo, List<Transition> transitions) {
        for (Transition saved : transitions) {
            if (neo.isEqual(saved)) {
                return true;
            }
        }
        return false;
    }

}
