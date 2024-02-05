export interface ISwitchs {
    label: string;
    state: boolean;
}

export interface ITab {
    title: string;
    active: boolean;
    onActiveChange: (isActive: boolean) => void;
}
