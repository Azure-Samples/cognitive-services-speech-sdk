package Carbon.Util;


public interface EventHandler<T> {
    void onEvent(Object sender, T e);
}
