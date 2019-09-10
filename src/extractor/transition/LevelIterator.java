package extractor.transition;

import global.structure.State;
import global.structure.Transition;
import global.structure.Type;

import java.util.List;

public class LevelIterator {
    //Attributes
    private int scopeLevel;
    private TransitionTools tools;

    //Constructor
    public LevelIterator (int scopeLevel) {
        this.scopeLevel = scopeLevel;
        tools = new TransitionTools();
    }

    //Public Methods
    public Transition iterate(Transition transition, List<State> states, List<State> functions, List<Transition> transitions) {
        Transition next = process(transition, functions, states, transitions);
        if (next.getTo() != null && next.getTo().getScopeLevel() > scopeLevel) {
            iterate(next, states, functions, transitions);
        }
        return next;
    }

    //Private Methods
    private Transition process(Transition transition, List<State> functions, List<State> states, List<Transition> transitions) {
        switch (transition.getTo().getElement()) {
            case ATTRIBUTION:
            case STATEMENT:
                transitions.add(transition);
                tools.findFunctionCall(transition, functions, states, transitions);
                State next = tools.getNextState(transition.getTo(), states);
                return tools.createTransition("lambda",transition.getTo(),next);
            case LOOP:
            case DECISION:
            default:
                State nextState = tools.getNextState(transition.getTo(), states);
                return tools.createTransition(transition.getEvent().getEvent(),transition.getFrom(),nextState);
        }



        /*if (state.getType() == Type.STATE) {
            switch (state.getElement()) {
                case JUMP:
                    tools.findFunctionCall(state, functions, states, transitions);
                    //Jump to somewhere
                    break;
                case ATTRIBUTION:
                case STATEMENT:
                    tools.findFunctionCall(state, functions, states, transitions);
                    State destiny = tools.getNextState(state, states);
                    destiny = tools.validateNextState(destiny, states, scopeLevel);
                    tools.createTransition("lambda", state, destiny, transitions);
                    break;
                default:
            }
        } else {
            switch (state.getElement()) {
                case LOOP:
                    tools.findFunctionCall(state, functions, states, transitions);
                    switch (state.getLabel()) {
                        case "for":
                            tools.decisionLoopTransition(state, states, transitions, scopeLevel);
                            break;
                        case "do":

                            break;
                        case "while":
                            break;
                        case "switch":
                            //System.out.println("S W I T C H   L O O P");
                            break;
                    }
                    break;
                case DECISION:
                    tools.findFunctionCall(state, functions, states, transitions);
                    tools.decisionLoopTransition(state, states, transitions, scopeLevel);
                    break;
                case EXCEPTION:
                    tools.findFunctionCall(state, functions, states, transitions);
                    break;
                case NAMESPACE:
                    break;
                case CLASS:
                    break;
                case STRUCT:
                    break;
                case FUNCTION:
                    break;
                case CONSTRUCTOR:
                    break;
                case DESTRUCTOR:
                    break;
                case OPERATOR:
                    break;
                default:
            }
        }*/
    }

}
