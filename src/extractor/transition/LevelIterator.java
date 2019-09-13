package extractor.transition;

import global.structure.State;
import global.structure.Transition;

import java.util.List;

public class LevelIterator {
    //Attributes
    private int scopeLevel;
    private TransitionTools tools;
    private State doLoop;

    //Constructor
    public LevelIterator (int scopeLevel) {
        this.scopeLevel = scopeLevel;
        tools = new TransitionTools();
        doLoop = null;
    }

    //Public Methods
    public Transition iterate(Transition transition, List<State> states, List<State> functions, List<Transition> transitions) {
        do {
            transition = process(transition, functions, states, transitions);
        } while (transition.getTo() != null && transition.getTo().getScopeLevel() > scopeLevel);
        return transition;
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
            case DECISION:
                transitions.add(transition);
                tools.findFunctionCall(transition, functions, states, transitions);
                //return tools.decisionLoop(transition, states, functions, transitions);
            case LOOP:
                switch (transition.getTo().getLabel()) {
                    case "for":
                        transitions.add(transition);
                        tools.findFunctionCall(transition, functions, states, transitions);
                        //return tools.decisionLoop(transition, states, functions, transitions);
                    case "do":
                        LevelIterator levelIterator = new LevelIterator(transition.getTo().getScopeLevel());
                        doLoop = tools.getNextState(transition.getTo(), states);
                        return levelIterator.iterate(
                                tools.createTransition(
                                        "lambda",
                                        transition.getFrom(),
                                        doLoop
                                ),
                                states,
                                functions,
                                transitions
                        );
                    case "while":
                        transitions.add(transition);
                        tools.findFunctionCall(transition, functions, states, transitions);
                        if (doLoop != null) {
                            transitions.add(tools.createTransition("TRUE",transition.getTo(), doLoop));
                            doLoop = null;
                            return tools.createTransition(
                                    "FALSE",
                                    transition.getTo(),
                                    tools.getNextState(transition.getTo(),states)
                            );
                        } else {
                            //return tools.decisionLoop(transition, states, functions, transitions);
                        }
                }
            default:
                State nextState = tools.getNextState(transition.getTo(), states);
                return tools.createTransition(transition.getEvent().getEvent(),transition.getFrom(),nextState);
        }
    }

}

