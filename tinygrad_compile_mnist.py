from typing import List, Callable
from pathlib import Path
from extra.datasets import fetch_mnist
from tinygrad.tensor import Tensor
from extra.export_model import export_model
from tinygrad import Tensor, TinyJit, nn
from tqdm import trange
import ssl

ssl._create_default_https_context = ssl._create_unverified_context

class TinyMnistModel:
    def __init__(self):
        input_size = 784
        hidden_size = 400
        num_classes = 10
        self.layers: List[Callable[[Tensor], Tensor]] = [
            # lambda x: x.flatten(1),
            nn.Linear(input_size, hidden_size),
            Tensor.relu,
            nn.Linear(hidden_size, num_classes),
        ]

    def __call__(self, x: Tensor) -> Tensor:
        return x.sequential(self.layers)


if __name__ == "__main__":
    X_train, Y_train, X_test, Y_test = fetch_mnist(tensors=True)
    X_train = X_train.flatten(1)
    X_test = X_test.flatten(1)

    model = TinyMnistModel()
    opt = nn.optim.Adam(nn.state.get_parameters(model))

    @TinyJit
    def train_step() -> Tensor:
        with Tensor.train():
            opt.zero_grad()
            samples = Tensor.randint(512, high=X_train.shape[0])
            loss = (
                model(X_train[samples])
                .sparse_categorical_crossentropy(Y_train[samples])
                .backward()
            )
            opt.step()
            return loss

    @TinyJit
    def get_test_acc() -> Tensor:
        return (model(X_test).argmax(axis=1) == Y_test).mean() * 100

    test_acc = float("nan")
    for i in (t := trange(70)):
        loss = train_step()
        if i % 10 == 9:
            test_acc = get_test_acc().item()
        t.set_description(f"loss: {loss.item():6.2f} test_accuracy: {test_acc:5.2f}%")

    mode = "clang"
    program, inp_sizes, out_sizes, state = export_model(
        model,
        mode,
        # 28x28 pixels
        Tensor.randn(28 * 28),
    )
    dirname = Path(__file__).parent

    # Make weight arrays const so they can be stored in flash
    program = program.replace("unsigned char buf", "const unsigned char buf")

    open("minst.c", "w").write(program)
