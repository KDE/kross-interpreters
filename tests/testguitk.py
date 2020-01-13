#!/usr/bin/env kross


class TkTest:
    def __init__(self):
        import tkinter
        self.root = tkinter.Tk()
        self.root.title("TkTest")
        self.root.deiconify()

        self.mainframe = tkinter.Frame(self.root)
        self.mainframe.pack()

        self.button1 = tkinter.Button(self.mainframe, text="Button1", command=self.callback1)
        self.button1.pack(side=tkinter.LEFT)

        self.button2 = tkinter.Button(self.mainframe, text="Button2", command=self.callback2)
        self.button2.pack(side=tkinter.LEFT)

        self.exitbutton = tkinter.Button(self.mainframe, text="Exit", command=self.root.destroy)
        self.exitbutton.pack(side=tkinter.LEFT)

        self.root.mainloop()

    def callback1(self):
        import tkinter.messagebox
        tkinter.messagebox.showinfo("Callback1", "Callback1 called.")

    def callback2(self):
        import tkinter.messagebox
        tkinter.messagebox.showinfo("Callback2", "Callback2 called.")


TkTest()
