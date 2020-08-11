/**
 * Bayesian linear regression model with conjugate normal-inverse-gamma
 * prior.
 *
 * The model is given by:
 *   $$\begin{align}
 *   \sigma^2 &\sim \mathrm{Inv\text{-}Gamma}(3, 4/10) \\
 *   \boldsymbol{\beta} &\sim \mathcal{N}(0, I\sigma^2) \\
 *   y_n &\sim \mathcal{N}(\mathbf{x}_n^{\top}\boldsymbol{\beta}, \sigma^2)
 *   \end{align}$$
 * The parameters are the noise variance $\sigma^2$ and vector of
 * coefficients $\boldsymbol{\beta}$. The data consists of observations $y_n$
 * and explanatory variables $\mathbf{x}_n$ for $n=1,\ldots,N$.
 */
class LinearRegressionModel < Model {
  /**
   * Explanatory variables.
   */
  X:Real[_,_];

  /**
   * Regression coefficients.
   */
  β:Random<Real[_]>;

  /**
   * Observation variance.
   */
  σ2:Random<Real>;

  /**
   * Observations.
   */
  y:Random<Real[_]>;
  
  fiber simulate() -> Event {
    N:Integer <- rows(X);
    P:Integer <- columns(X);
    if (N > 0 && P > 0) {
      σ2 ~ InverseGamma(3.0, 0.4);
      assert !σ2.hasValue();
      β ~ Gaussian(vector(0.0, P), identity(P), σ2);
      assert !σ2.hasValue();
      assert !β.hasValue();
      y ~ Gaussian(X*β, σ2);
      assert !σ2.hasValue();
      assert !β.hasValue();
    }
  }
  
  function read(buffer:Buffer) {
    X <-? buffer.getRealMatrix("X");
    y <-? buffer.getRealVector("y");
  }
  
  function write(buffer:Buffer) {
    buffer.set("beta", β);
    buffer.set("sigma2", σ2);
  }
}