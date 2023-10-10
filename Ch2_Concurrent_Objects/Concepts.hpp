template <typename T>
concept LockConcept = requires (T t) {
    { t.lock() };
    { t.unlock() };
};
