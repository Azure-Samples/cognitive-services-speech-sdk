
const WaitForCondition = (condition: () => boolean, after: () => void): void => {
    if (condition() === true) {
        after();
    } else {
        setTimeout(() => WaitForCondition(condition, after), 500);
    }
};

export default WaitForCondition;
