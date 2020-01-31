package extractor.transition;

import global.structure.State;
import global.structure.Transition;
import global.tools.Statistics;
import global.tools.StringTools;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.ArrayList;
import java.util.List;

public class Iterator {
    //Attributes
    private StringTools stringTools;
    private Statistics statistics;
    private TransitionTools tools;
    private TryStorage tryStorage;
    private BreakControl breakControl;
    private ReturnControl returnControl;
    private boolean functionLevel;
    private boolean loopLevel;
    private boolean stop;
    private int scopeLevel;
    private State state;

    //Constructor
    public Iterator (State state, int scopeLevel) {
        stringTools = new StringTools();
        statistics = Statistics.getInstance();
        tools = new TransitionTools();
        tryStorage = TryStorage.getInstance();
        breakControl = BreakControl.getInstance();
        returnControl = ReturnControl.getInstance();
        functionLevel = false;
        loopLevel = false;
        stop = false;
        this.scopeLevel = scopeLevel;
        this.state = state;
    }

    //Methods
    public List<Transition> iterate (List<Transition> response, List<State> states, List<Transition> transitions, List<State> functions) {
        //DEBUG
        /*for (ParseTree element : state.getLine().getContent()) {
            System.out.print(element.getText());
        }
        System.out.println();*/

        List<Transition> newResponse;
        do {
            newResponse = (process (response, states, transitions, functions));
            response = newResponse;
            if (breakControl.getBreak()) {
                if (loopLevel) {
                    newResponse.addAll(breakControl.getResponse());
                    breakControl.reset();
                }
            }
            if (stop) {
                break;
            }
        } while (state.getScopeLevel() > scopeLevel);
        if (returnControl.getReturn()) {
            if (functionLevel) {
                newResponse.addAll(returnControl.getResponse());
                returnControl.reset();
            }
        }
        return newResponse;
    }

    private List<Transition> process (List<Transition> response, List<State> states, List<Transition> transitions, List<State> functions) {
        List<Transition> outsideResponse = new ArrayList<>();
        switch (state.getElement()) {
            case FUNCTION:
            case OPERATOR:
                functionLevel = true;
                state = tools.getNextState(state, states);
                return response;
            case JUMP:
                if (state.getLabel().equals("break")) {
                    breakControl.setBreak();
                    breakControl.addResponse(response);
                }
                else if (state.getLabel().equals("return")) {
                    tools.finalizeTransition(response, state, transitions);
                    tools.finalizeTransition(functionCall(response, states, transitions, functions), state, transitions);
                    tryStorage.store(tools.createTransition("exception", state, null));
                    List<Transition> returnResponse = new ArrayList<>();
                    returnResponse.add(tools.createTransition(extractReturnEvent(state), state, null));
                    returnControl.setReturn();
                    returnControl.addResponse(returnResponse);
                }
                stop = true;
                state = tools.getNextState(state, states);
                break;
            case ATTRIBUTION:
            case STATEMENT:
                tools.finalizeTransition(response, state, transitions);
                tools.finalizeTransition(functionCall(response, states, transitions, functions), state, transitions);
                tryStorage.store(tools.createTransition("exception", state, null));
                String event = tools.extractEvent(state.getLine());
                outsideResponse.add(tools.createTransition(event, state, null));
                state = tools.getNextState(state, states);
                break;
            case DECISION:
                tools.finalizeTransition(response, state, transitions);
                tools.finalizeTransition(functionCall(response, states, transitions, functions), state, transitions);
                tryStorage.store(tools.createTransition("exception", state, null));
                if (state.getLabel().equals("if")) {
                    while (true) {
                        List<Transition> falseResponse = new ArrayList<>();
                        falseResponse.add(tools.createTransition("FALSE", state, null));
                        outsideResponse.addAll(iterateLoop(states, transitions, functions));
                        state = tools.getNextSameLevelState(state, states);
                        if (!state.getLabel().equals("else_if") && !state.getLabel().equals("else")) {
                            outsideResponse.addAll(falseResponse);
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
                loopLevel = true;
                tools.finalizeTransition(response, state, transitions);
                tools.finalizeTransition(functionCall(response, states, transitions, functions), state, transitions);
                tryStorage.store(tools.createTransition("exception", state, null));
                if (state.getLabel().equals("do")) {
                    tools.finalizeTransition(iterateLoop(states, transitions, functions), tools.getNextSameLevelState(state, states), transitions);
                    transitions.add(tools.createTransition("FALSE", tools.getNextSameLevelState(state, states), state));
                    outsideResponse.add(tools.createTransition("TRUE", tools.getNextSameLevelState(state, states), null));
                    state = tools.getNextState(tools.getNextSameLevelState(state, states), states);
                    break;
                }
                if (state.getLabel().equals("while")) {
                    outsideResponse.add(tools.createTransition("FALSE", state, null));
                    //outsideResponse.addAll(iterateLoop(states, transitions, functions));
                    tools.finalizeTransition(iterateLoop(states, transitions, functions), state, transitions);
                    state = tools.getNextSameLevelState(state, states);
                    break;
                }
                if (state.getLabel().equals("for")) {
                    outsideResponse.add(tools.createTransition("FALSE", state, null));
                    //outsideResponse.addAll(iterateLoop(states, transitions, functions));
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
                            statistics.addStateTransition();
                            innerResponse.add(tools.createTransition(extractReturnEvent(state), me, null));
                            state = tools.getNextState(state, states);
                        }
                        if (state.getLabel().equals("default")) {
                            //statistics.addStateTransition();
                            innerResponse.add(tools.createTransition("lambda", me, null));
                            state = tools.getNextState(state, states);
                        } else {
                            Iterator iterator = new Iterator(state, me.getScopeLevel());
                            outsideResponse.addAll(iterator.iterate(innerResponse, states, transitions, functions));
                            innerResponse.clear();
                            state = tools.getNextSameLevelDecision(state, states);
                        }
                    }
                    break;
                }
            case EXCEPTION:
                if (state.getLabel().equals("try")) {
                    tryStorage.activeTry();
                    tools.finalizeTransition(response, state, transitions);
                    List<Transition> tryResponse = new ArrayList<>();
                    tryResponse.add(tools.createTransition("lambda", state, null));
                    Iterator iterator = new Iterator(tools.getNextState(state, states), state.getScopeLevel());
                    outsideResponse.addAll(iterator.iterate(tryResponse, states, transitions, functions));
                    state = tools.getNextSameLevelState(state, states);
                    tryStorage.deactiveTry();
                    while (state.getLabel().equals("catch")) {
                        tools.finalizeTransition(tryStorage.getResponse(), state, transitions);
                        List<Transition> catchResponse = new ArrayList<>();
                        catchResponse.add(tools.createTransition("lambda", state, null));
                        Iterator catchIterator = new Iterator(tools.getNextState(state, states), state.getScopeLevel());
                        outsideResponse.addAll(catchIterator.iterate(catchResponse, states, transitions, functions));
                        state = tools.getNextSameLevelState(state, states);
                    }
                    tryStorage.reset();
                    break;
                }
            default:
                state = tools.getNextState(state, states);
                return response;
        }
        return outsideResponse;
    }

    private List<Transition> functionCall (List<Transition> response, List<State> states, List<Transition> transitions, List<State> functions) {
        List<Transition> newResponse = new ArrayList<>();
        boolean external = false;
        for (ParseTree key : state.getLine().getContent()) {
            for (State function : functions) {
                if (function.getLabel().equals(key.getText()) && !external) {
                    Iterator iterator = new Iterator(function, function.getScopeLevel());
                    newResponse.addAll(iterator.iterate(response, states, transitions, functions));
                }
            }
            if (key.getText().equals(".") || key.getText().equals("->") || key.getText().equals("::")) {
                external = true;
            } else {
                external = false;
            }
        }
        return newResponse;
    }

    private boolean imInside (State me) {
        return state.getScopeLevel() == me.getScopeLevel() + 1;
    }

    private String extractReturnEvent(State state) {
        try {
            if (stringTools.noSpecialCharacters(state.getLine().getContent().get(1).getText()).equals("")) {
                for (ParseTree element : state.getLine().getContent()) {
                    if (!stringTools.noSpecialCharacters(element.getText()).isEmpty()) {
                        return stringTools.noSpecialCharacters(element.getText());
                    }
                }
                return "lambda";
            }
            return stringTools.noSpecialCharacters(state.getLine().getContent().get(1).getText());
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
