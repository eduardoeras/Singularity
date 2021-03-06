package extractor.transition;

import global.structure.*;
import global.tools.Statistics;

import java.util.ArrayList;
import java.util.List;

public class TransitionWalker {
    //Attributes
    private List<State> main;
    private List<State> functions;
    private TransitionTools tools;
    private Transition last;
    private Statistics statistics;

    //Constructor
    public TransitionWalker () {
        main = new ArrayList<>();
        functions = new ArrayList<>();
        tools = new TransitionTools();
        last = tools.createInitialState();
        statistics = Statistics.getInstance();
    }

    //Public Methods
    public void walk (List<State> states, List<Transition> transitions) {
        createMainFunction(states);
        collectFunctions(states);
        loop(states, transitions);
        main.clear();
    }

    //Private Methods
    private void createMainFunction (List<State> states) {
        for (State state : states) {
            switch (state.getElement()) {
                case CONSTRUCTOR:
                case FUNCTION:
                case OPERATOR:
                    if (state.getVisibility() != Visibility.PRIVATE) {
                        main.add(state);
                        statistics.addComponent();
                    }
                    break;
            }
        }
    }

    private void collectFunctions(List<State> states) {
        for (State state : states) {
            if (state.getElement() == Element.FUNCTION) {
                functions.add(state);
            }
        }
    }

    private void loop (List<State> states, List<Transition> transitions) {
        List<Transition> response = new ArrayList<>();
        response.add(tools.createTransition(last.getEvent().getEvent(), last.getFrom(), null));
        for (State destiny : main) {
            Iterator iterator = new Iterator(destiny, destiny.getScopeLevel());
            response = iterator.iterate(response, states, transitions, functions);
        }
        tools.finalizeTransition(response, tools.createFinalState(), transitions);
    }

}
